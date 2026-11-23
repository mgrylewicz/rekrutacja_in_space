#include "CANSimulator.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

bool CompareMessages::operator()(const CANMessage& a, const CANMessage& b) const{
    if(a.id != b.id){
        return a.id > b.id;
    }

    return a.generation_time_us > b.generation_time_us;
}

CANSimulator::CANSimulator( std::vector<MessageSource> sources, uint64_t generation_end_us):
                            sources_(std::move(sources)), generation_end_us_(generation_end_us){

    // weryfikacja podstawowej poprawności konfiguracji
    for (const auto& source : sources_) {

        if (source.id > 0x7FF) {
            throw std::invalid_argument(
                "CAN 2.0A identifier must fit in 11 bits"
            );
        }

        if (source.data_size > 8) {
            throw std::invalid_argument(
                "Classic CAN supports at most 8 data bytes"
            );
        }

        if (source.period_us == 0) {
            throw std::invalid_argument(
                "Message period must be greater than zero"
            );
        }

        // w tym modelu każde ID oznacza jeden typ wiadomości
        if (!statistics_.emplace(source.id, MessageStats{}).second) {
            throw std::invalid_argument(
                "Duplicate CAN identifier"
            );
        }
    }
}

CANMessage CANSimulator::generate_message(const MessageSource& source) const{
    std::optional<uint64_t> deadline = std::nullopt;

    if(source.relative_deadline_us.has_value()){
        deadline = source.next_generation_us + source.relative_deadline_us.value();
    }

    return{
        source.id,
        source.data_size,
        source.next_generation_us,
        deadline
    };
}

uint64_t CANSimulator::calculate_transmission_time(uint8_t data_size) const{
    constexpr uint64_t bitrate = 500000;

    const uint64_t base_bits = 47 + 8 * data_size;// ramka CAN 2.0A wraz z odstępem międzyramkowym
    const uint64_t stuffable_bits = 34 + 8 * data_size;// część ramki objęta bit stuffingiem
    const uint64_t stuffing_bits = (stuffable_bits - 1)/4;// konserwatywne oszacowanie narzutu bit stuffingu
    const uint64_t total_bits = base_bits + stuffing_bits;

    return (total_bits * 1000000ULL + bitrate - 1) / bitrate;
}

void CANSimulator::generate_pending_messages(){

    for(auto& source : sources_){

        if(source.next_generation_us == simulation_time_us_ and simulation_time_us_ < generation_end_us_){
            message_queue_.push(generate_message(source));
            statistics_[source.id].generated_count++;
            source.next_generation_us += source.period_us;
        }
    }

    max_queue_size_ = std::max(max_queue_size_, message_queue_.size());
}

void CANSimulator::finish_transmission(){
    if(!current_message_.has_value() or simulation_time_us_ != transmission_end_us_){
        return;
    }

    const CANMessage& message = current_message_.value();

    const uint64_t delay_us = simulation_time_us_ - message.generation_time_us;

    MessageStats& stats = statistics_[message.id];

    stats.sent_count++;
    stats.total_delay_us += delay_us;
    stats.max_delay_us = std::max(stats.max_delay_us, delay_us);

    if(message.deadline_us.has_value() and simulation_time_us_ > message.deadline_us.value()){

        stats.missed_deadlines++;
    }

    // magistrala staje się wolna
    current_message_.reset();
}

void CANSimulator::start_transmission(){

    // nie rozpoczynamy nowej transmisji, jeśli magistrala
    // jest zajęta albo kolejka jest pusta
    if (current_message_.has_value() or message_queue_.empty()){
        return;
    }

    current_message_ = message_queue_.top();
    message_queue_.pop();

    transmission_end_us_ = simulation_time_us_ + calculate_transmission_time(current_message_->data_size);
}

std::optional<uint64_t> CANSimulator::find_next_event() const{
    std::optional<uint64_t> next_event;

    // szukamy najbliższego wygenerowania wiadomości
    for(const auto& source : sources_){

        if(source.next_generation_us < generation_end_us_){

            if(!next_event.has_value() or source.next_generation_us < next_event.value()){
                next_event = source.next_generation_us;
            }
        }
    }

    // uwzględniamy koniec aktualnej transmisji
    if(current_message_.has_value()){

        if(!next_event.has_value() or transmission_end_us_ < next_event.value()){
            next_event = transmission_end_us_;
        }
    }

    return next_event;
}

void CANSimulator::run(){
    while(true){
        generate_pending_messages();
        finish_transmission();
        start_transmission();

        const auto next_event = find_next_event();

        if(!next_event.has_value()){
            break;
        }

        simulation_time_us_ = next_event.value();
    }
}

void CANSimulator::print_results() const{
    std::cout<<"\n=== CAN Simulation Results ===\n";
    std::cout<<"Generation duration: "<<generation_end_us_ / 1000<<" ms\n\n";

    for(const auto& source : sources_){
        const MessageStats& stats = statistics_.at(source.id);
        double average_delay_us = 0.0;

        if(stats.sent_count > 0){
            average_delay_us = static_cast<double>(stats.total_delay_us) / stats.sent_count;
        }

        std::cout<<"CAN ID: 0x"<<std::hex<<std::setw(3)<<std::setfill('0')<<source.id<< std::dec <<"\n";

        std::cout<<"  Generated messages: "<<stats.generated_count<<"\n";

        std::cout<<"  Sent messages: "<<stats.sent_count<<"\n";

        std::cout<<"  Average delay: "<<std::fixed<<std::setprecision(2)<<average_delay_us<<" us\n";

        std::cout<<"  Maximum delay: "<<stats.max_delay_us<<" us\n";

        std::cout<<"  Missed deadlines: "<< stats.missed_deadlines<<"\n\n";
    }

    std::cout<<"Maximum queue size: "<< max_queue_size_ <<"\n";
    std::cout<< "Final simulation time: "<< simulation_time_us_ <<" us\n";
}