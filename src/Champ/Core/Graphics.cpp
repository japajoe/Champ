#include "Graphics.hpp"
#include "OpenGL.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "../Shaders/ScreenShader.hpp"
#include <imgui/imgui_manager.h>
#include <cstdint>
#include <vector>
#include <iostream>

namespace Champ
{
    static uint32_t gScreenVAO = 0;
    static Shader gScreenShader;
    static int32_t gWidth = 0;
    static int32_t gHeight = 0;
    static bool gInvalidate = false;
    static std::vector<FrameBuffer> gFramebuffers;
    static std::vector<PostProcessor *> gPostProcessors;
    static PingPongBuffer gPingpongBuffer;

    void Graphics::Initialize(int32_t width, int32_t height, GLFWwindow *window)
    {
        gWidth = width;
        gHeight = height;

#ifdef __EMSCRIPTEN__
        FrameBufferTextureFormat colorFormat = FrameBufferTextureFormat::RGBA8;
#else
        FrameBufferTextureFormat colorFormat = FrameBufferTextureFormat::RGBA16F;
#endif

        FrameBufferTextureSpecification colorAttachment = {
            .format = colorFormat,
            .wrap = TextureWrapMode::ClampToEdge,
            .filter = TextureFilterMode::Linear};

        FrameBufferTextureSpecification depthAttachment = {
            .format = FrameBufferTextureFormat::Depth24Stencil8,
            .wrap = TextureWrapMode::ClampToEdge,
            .filter = TextureFilterMode::Linear};

        FrameBufferSpecification specMain = {
            .width = (uint32_t)gWidth,
            .height = (uint32_t)gHeight,
            .samples = 4,
            .resizable = true,
            .attachments = {
                colorAttachment, // Main color
                colorAttachment, // Brightness
                depthAttachment
            }
        };

        FrameBufferSpecification specScreen1 = {
            .width = (uint32_t)gWidth,
            .height = (uint32_t)gHeight,
            .samples = 1,
            .resizable = true,
            .attachments = {
                colorAttachment, // Main color
                colorAttachment, // Brightness
                depthAttachment
            }
        };

        FrameBufferSpecification specScreen2 = {
            .width = (uint32_t)gWidth,
            .height = (uint32_t)gHeight,
            .samples = 1,
            .resizable = true,
            .attachments = {
                colorAttachment, // Main color
            }
        };


        gFramebuffers.resize(3);

        gFramebuffers[0].Generate(specMain);
        gFramebuffers[1].Generate(specScreen1); // Used to present to screen and post processing
        gFramebuffers[2].Generate(specScreen2); // Used to present to screen and post processing

        glGenVertexArrays(1, &gScreenVAO);

        try
        {
            gScreenShader.Generate(ScreenShader::GetVertexSource(), ScreenShader::GetFragmentSource());
        }
        catch (const std::exception &ex)
        {
            std::cout << ex.what() << '\n';
        }

        ImGui::Manager::Initialize(window);
    }

    void Graphics::Destroy()
    {
        ImGui::Manager::Destroy();
    }

    void Graphics::NewFrame()
    {
        InvalidateFramebuffers();
        gFramebuffers[0].Bind();
    }

    void Graphics::EndFrame()
    {
        gFramebuffers[0].Unbind();
        gFramebuffers[0].Blit(gFramebuffers[1], BlitOption_Color | BlitOption_Depth);
        gPingpongBuffer.src = &gFramebuffers[1];
        gPingpongBuffer.dst = &gFramebuffers[2];

        if (gPostProcessors.size() > 0)
        {
            Matrix4 identity = glm::identity<Matrix4>();

            for (size_t i = 0; i < gPostProcessors.size(); i++)
            {
                if (!gPostProcessors[i]->IsActive())
                    continue;
                gPostProcessors[i]->OnProcess(identity, identity);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        gScreenShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPingpongBuffer.src->GetColorAttachment(0));
        gScreenShader.SetInt("uTexture", 0);

        glBindVertexArray(gScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    void Graphics::BeginGUI()
    {
        ImGui::Manager::BeginFrame();
    }

    void Graphics::EndGUI()
    {
        ImGui::Manager::EndFrame();
    }

    void Graphics::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        gWidth = width;
        gHeight = height;
        gInvalidate = true;
    }

    int32_t Graphics::GetScreenWidth()
    {
        return gWidth;
    }

    int32_t Graphics::GetScreenHeight()
    {
        return gHeight;
    }

    void Graphics::InvalidateFramebuffers()
    {
        if (gInvalidate)
        {
            gInvalidate = false;

            glViewport(0, 0, gWidth, gHeight);

            for (size_t i = 0; i < gFramebuffers.size(); i++)
                gFramebuffers[i].Resize(gWidth, gHeight);
        }
    }

    void Graphics::AddPostProcessor(PostProcessor *processor)
    {
        if (!processor)
            return;

        processor->vao = gScreenVAO;
        processor->pingpongBuffer = &gPingpongBuffer;
        processor->Initialize();
        gPostProcessors.push_back(processor);
    }
}