#include "Utils.hpp"
#include <cstdlib>
#include <ctime>

namespace Logi
{

    std::vector<std::byte> generateRandomBuffer(uint64_t size)
    {
        std::vector<std::byte> buffer;
        buffer.resize(size);
        srand((unsigned) time(0));
        for(auto& byte : buffer)
        {
            byte = static_cast<std::byte>(rand() % 256);
        }
        return buffer;
    }

    std::vector<bool> generateRandomFlags(uint16_t size)
    {
        std::vector<bool> flags;
        srand((unsigned) time(0));
        for (size_t i = 0; i < size; i++)
        {
            flags.emplace_back(rand() % 2);
        }
        return flags;
    }

    Endianess checkHostEndianess()
    {
        unsigned int i = 1;
        char* c = (char*)&i;
        return (*c) ? Endianess::LittleEndian : Endianess::BigEndian;
    }

    uint16_t byteSwap16(uint16_t x)
    {
        x = ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
        return x;
    };

    uint32_t byteSwap32(uint32_t x)
    {
        x = ((x & 0x000000FF) << 24) |
            ((x & 0x0000FF00) <<  8) |
            ((x & 0x00FF0000) >>  8) |
            ((x & 0xFF000000) >> 24);
        return x;
    };

    uint16_t readField16(uint8_t b0, uint8_t b1, bool swap)
    {
        uint16_t x = b0 | (b1 << 8);
        if (swap)
            x = byteSwap16(x);
        return x; 
    }

    uint32_t readField32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, bool swap)
    {
        uint16_t x = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        if (swap)
            x = byteSwap32(x);
        return x; 
    }

    bool readBit(uint8_t b, unsigned int position)
    {
        return (b >> position) & 1;
    }


} // namespace Logi

