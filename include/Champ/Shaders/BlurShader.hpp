#pragma once

#include <string>

namespace Champ
{
    class BlurShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}