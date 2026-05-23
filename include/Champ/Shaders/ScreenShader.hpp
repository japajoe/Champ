#pragma once

#include <string>

namespace Champ
{
    class ScreenShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}