#include "CANSimulator.h"

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

int main(){

    // konfiguracja pięciu typów wiadomości
    // generowanych przez cztery moduły
    std::vector<MessageSource> sources = {
        // ID,    bytes, period, next generation, deadline
        {0x010,     4,   10000,         0,          50000},        // silnik - temperatura
        {0x020,     4,   20000,         0,          10000},        // silnik - ciśnienie
        {0x100,     2,   100000,        0,          std::nullopt}, // zasilanie - napięcie
        {0x200,     8,   200000,        0,          100000},       // nawigacja - pozycja GPS
        {0x300,     4,   500000,        0,          std::nullopt}  // diagnostyka - status systemu
    };

    // generowanie wiadomości przez 1 sekundę
    constexpr uint64_t generation_end_us = 1000000;

    CANSimulator simulator(sources, generation_end_us);

    simulator.run();
    simulator.print_results();

    return 0;
}