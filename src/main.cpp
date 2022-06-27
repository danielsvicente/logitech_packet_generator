#include "ConsolePrinter.hpp"
#include "IPrinter.hpp"
#include "PacketGenerator.hpp"
#include "Utils.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstddef>
#include <limits>
#include <memory>

int main() {

    uint8_t softwareId{69};
    unsigned int bufferSize{};
    Logi::ConsolePrinter console;
    Logi::PacketGenerator packetGenerator{softwareId, console};

    std::cout << "Logitech Packet Generator v1.0\n\n";

    while(true) {
        bool validInput{};
        do
        {
            std::cout << "Enter buffer size [1..1000 bytes]: ";
            while(!(std::cin >> bufferSize)){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Try again: ";
            }

            if ((bufferSize >= 1) && (bufferSize <= 1000))
            {
                std::cout << "\n";
                validInput = true;
            }
            else
            {
                std::cout << "Invalid buffer size.\n";
                validInput = false;
            }

        } while (!validInput);

        auto buffer = Logi::generateRandomBuffer(bufferSize);
        auto flags = Logi::generateRandomFlags(3);

        auto packets = packetGenerator.createPackets(buffer, {flags[0], flags[1], flags[2]});

        packetGenerator.printPackets(packets);
    }

    return 0;
}