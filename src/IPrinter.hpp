#pragma once

#include <string_view>

namespace Logi
{

    class IPrinter
    {
    public:

        virtual ~IPrinter() {};

        virtual void print(std::string_view str) = 0;
    };

} // namespace Logi