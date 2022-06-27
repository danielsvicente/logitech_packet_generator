#include "ConsolePrinter.hpp"
#include <iostream>

namespace Logi
{
    void ConsolePrinter::print(std::string_view str)
    {
        std::cout << str << "\n";
    }

} // namespace Logi