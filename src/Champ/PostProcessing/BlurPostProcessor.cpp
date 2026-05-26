#include "BlurPostProcessor.hpp"
#include "../Core/OpenGL.hpp"
#include "../Shaders/BlurShader.hpp"
#include <iostream>

namespace Champ
{
    void BlurPostProcessor::Initialize()
    {
        try
        {
            blurShader.Generate(BlurShader::GetVertexSource(), BlurShader::GetFragmentSource());
            shader = &blurShader;
        }
        catch(const std::exception &ex)
        {
            std::cout << ex.what() << '\n';
        }
    }

    void BlurPostProcessor::OnProcess(const Matrix4 &projection, const Matrix4 &view)
    {
        shader->Use();
        shader->SetInt("uTexture", 0);

        RunPass(true);
        RunPass(false);
    }

	void BlurPostProcessor::RunPass(bool horizontal)
	{
		Bind();

		shader->SetInt("uHorizontal", horizontal ? 1 : 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetInputTexture());

		Render();
		
		SwapBuffers();
	}
}