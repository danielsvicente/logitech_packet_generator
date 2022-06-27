#pragma once

#include "IPrinter.hpp"

namespace Logi
{

    class ConsolePrinter : public IPrinter
    {
    public:

        ConsolePrinter() = default;
        ~ConsolePrinter() = default;

        void print(std::string_view str) override;
    };

} // namespace Logi