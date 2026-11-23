#ifndef CAN_SIMULATOR_T
#define CAN_SIMULATOR_T

#include <cstdint>
#include <cstddef>
#include <map>
#include <optional>
#include <queue>
#include <vector>

// pojedyncza wiadomość oczekująca na transmisję
struct CANMessage{
    uint16_t id;
    uint8_t data_size;

    uint64_t generation_time_us;
    std::optional<uint64_t> deadline_us;
};

// konfiguracja okresowego źródła wiadomości
struct MessageSource {
    uint16_t id;
    uint8_t data_size;

    uint64_t period_us;
    uint64_t next_generation_us;

    std::optional<uint64_t> relative_deadline_us;
};

// statystyki dla jednego identyfikatora CAN
struct MessageStats {
    uint64_t generated_count = 0;
    uint64_t sent_count = 0;

    uint64_t total_delay_us = 0;
    uint64_t max_delay_us = 0;
    uint64_t missed_deadlines = 0;
};

// niższy identyfikator CAN oznacza wyższy priorytet
struct CompareMessages {
    bool operator()(const CANMessage& a, const CANMessage& b) const;
};

class CANSimulator {
        // konfiguracja źródeł
        std::vector<MessageSource> sources_;

        // kolejka wiadomości oczekujących na transmisję
        std::priority_queue<CANMessage, std::vector<CANMessage>, CompareMessages> message_queue_;

        // statystyki dla poszczególnych ID
        std::map<uint16_t, MessageStats> statistics_;

        // czas symulowany
        uint64_t simulation_time_us_ = 0;
        uint64_t generation_end_us_;
        uint64_t transmission_end_us_ = 0;

        // aktualnie transmitowana wiadomość
        std::optional<CANMessage> current_message_;

        // największa zaobserwowana długość kolejki
        size_t max_queue_size_ = 0;

        CANMessage generate_message(const MessageSource& source) const;

        uint64_t calculate_transmission_time(uint8_t data_size) const;

        void generate_pending_messages();
        void finish_transmission();
        void start_transmission();

        std::optional<uint64_t> find_next_event() const;

    public:
        // konstruktor przyjmuje konfigurację źródeł i czas generowania
        CANSimulator(std::vector<MessageSource> sources, uint64_t generation_end_us);
        void run();// uruchamia symulację
        void print_results() const;// wyświetla statystyki
};



#endif