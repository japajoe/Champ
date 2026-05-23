#pragma once

#include <string>

namespace Champ
{
    class VHSShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}