#pragma once

#include "../Core/PostProcessor.hpp"

namespace Champ
{
    class PixelatorPostProcessor : public PostProcessor
    {
    public:
        void Initialize() override;
        void OnProcess(const Matrix4 &projection, const Matrix4 &view) override;
        void SetScale(float scale);
        float GetScale() const;
    private:
        Shader pixelatorShader;
        float scale;
    };
}