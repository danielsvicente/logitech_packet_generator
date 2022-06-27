#include "PacketGenerator.hpp"
#include "Utils.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstddef>
#include <memory>

int main() {

    unsigned int bufferSize{0};

    std::cout << "Logitech Packet Generator v1.0\n\n";
    std::cout << "Enter buffer size: ";
    std::cin >> bufferSize;

    Logi::PacketGenerator packetGenerator{52};
    packetGenerator.createPackets(Logi::generateRandomBuffer(bufferSize), {false, false, false});

    return 0;
}