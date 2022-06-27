#pragma once

#include <cstddef>
#include <iostream>
#include <vector>

namespace Logi
{
    enum class Endianess: bool
    {
        BigEndian = true,
        LittleEndian = false
    };

    std::vector<std::byte> generateRandomBuffer(uint64_t size);
    std::vector<bool> generateRandomFlags(uint16_t size);

    Endianess checkHostEndianess();

    uint16_t byteSwap16(uint16_t x);
    uint32_t byteSwap32(uint32_t x);

    uint16_t readField16(uint8_t b0, uint8_t b1, bool swap);
    uint32_t readField32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, bool swap);

    bool readBit(uint8_t b, unsigned int position);

} // namespace Logi