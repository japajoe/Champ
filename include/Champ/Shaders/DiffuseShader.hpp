#pragma once

#include <string>

namespace Champ
{
    class DiffuseShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}