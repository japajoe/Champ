#pragma once

#include <string>

namespace Champ
{
    class SkyboxShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}