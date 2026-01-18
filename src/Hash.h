#pragma once

#include <string>

namespace gitz
{
    class Hash
    {
    public:
        static std::string sha1(const std::string& input);
    };
}
