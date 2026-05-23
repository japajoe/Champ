#pragma once

#include <string>

namespace Champ
{
    class PixelatorShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}