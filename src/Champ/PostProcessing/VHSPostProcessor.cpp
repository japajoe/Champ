#include "VHSPostProcessor.hpp"
#include "../Core/OpenGL.hpp"
#include "../Core/Time.hpp"
#include "../Shaders/VHSShader.hpp"
#include <iostream>

namespace Champ
{
    void VHSPostProcessor::Initialize()
    {
        time = 0.0f;

        try
        {
            vhsShader.Generate(VHSShader::GetVertexSource(), VHSShader::GetFragmentSource());
            shader = &vhsShader;
        }
        catch(const std::exception &ex)
        {
            std::cout << ex.what() << '\n';
        }
    }

    void VHSPostProcessor::OnProcess(const Matrix4 &projection, const Matrix4 &view)
    {
        Bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GetInputTexture());

        shader->Use();
        shader->SetInt("uTexture", 0);
        shader->SetFloat("uTime", time);

        Render();
        
        SwapBuffers();

        time += Time::GetDeltaTime();

        if(time >= 30.0)
            time -= 30.0;
    }
}