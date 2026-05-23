#include "PixelatorPostProcessor.hpp"
#include "../Core/OpenGL.hpp"
#include "../Shaders/PixelatorShader.hpp"
#include <iostream>

namespace Champ
{
    void PixelatorPostProcessor::Initialize()
    {
        scale = 4.0f;

        try
        {
            pixelatorShader.Generate(PixelatorShader::GetVertexSource(), PixelatorShader::GetFragmentSource());
            shader = &pixelatorShader;
        }
        catch(const std::exception &ex)
        {
            std::cout << ex.what() << '\n';
        }
    }

    void PixelatorPostProcessor::OnProcess(const Matrix4 &projection, const Matrix4 &view)
    {
        Bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GetInputTexture());

        shader->Use();
        shader->SetInt("uTexture", 0);
        shader->SetFloat("uPixelSize", scale);

        Render();
        
        SwapBuffers();
    }

    void PixelatorPostProcessor::SetScale(float scale)
    {
        this->scale = scale;
    }

    float PixelatorPostProcessor::GetScale() const
    {
        return scale;
    }
}