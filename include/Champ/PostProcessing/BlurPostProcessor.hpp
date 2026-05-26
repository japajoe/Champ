#pragma once

#include "../Core/PostProcessor.hpp"

namespace Champ
{
    class BlurPostProcessor : public PostProcessor
    {
    public:
        void Initialize() override;
        void OnProcess(const Matrix4 &projection, const Matrix4 &view) override;
    private:
        Shader blurShader;
        void RunPass(bool horizontal);
    };
}