#pragma once

#include "PostProcessor.hpp"
#include <cstdint>

struct GLFWwindow;

namespace Champ
{
    class Graphics
    {
        friend class ApplicationBase;

    public:
        static int32_t GetScreenWidth();
        static int32_t GetScreenHeight();
        static void AddPostProcessor(PostProcessor *processor);

    private:
        static void Initialize(int32_t width, int32_t height, GLFWwindow *window);
        static void Destroy();
        static void NewFrame();
        static void EndFrame();
        static void BeginGUI();
        static void EndGUI();
        static void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
        static void InvalidateFramebuffers();
    };
}