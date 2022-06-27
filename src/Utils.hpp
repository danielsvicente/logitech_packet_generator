#pragma once

#include <cstddef>
#include <vector>
#include <iostream>

namespace Logi
{
    enum class Endianess: bool
    {
        BigEndian = true,
        LittleEndian = false
    };

    std::vector<std::byte> generateRandomBuffer(uint16_t size);

    Endianess checkHostEndianess();

} // namespace Logi