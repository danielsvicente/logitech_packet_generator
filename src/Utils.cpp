#include "Utils.hpp"
#include <cstdlib>
#include <ctime>

namespace Logi
{

    std::vector<std::byte> generateRandomBuffer(uint16_t size)
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

    Endianess checkHostEndianess()
    {
        unsigned int i = 1;
        char* c = (char*)&i;
        return (*c) ? Endianess::LittleEndian : Endianess::BigEndian;
    }

} // namespace Logi

