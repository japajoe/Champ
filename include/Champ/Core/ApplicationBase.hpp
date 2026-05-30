#pragma once

#include "Image.hpp"
#include <string>
#include <cstdint>

struct GLFWwindow;

namespace Champ
{
    struct ApplicationSettings
    {
        int32_t width;
        int32_t height;
        std::string title;
        bool vsync;
        bool fullScreen;
        bool guiDocking;
        bool guiViewports;
        Image *icon;
    };

    class ApplicationBase
    {
    public:
        ApplicationBase(int32_t width, int32_t height, const std::string &title, bool vsync);
        ApplicationBase(ApplicationSettings *pSettings);
        int Run();
        GLFWwindow *GetNativeWindow() const;
        void SetWindowLimits(int32_t minWidth = -1, int32_t minHeight = -1, int32_t maxWidth = -1, int32_t maxHeight = -1);
        void SetTitle(const std::string &title);
        void SetCursor(bool visible);
        void Quit();
        
    protected:
        virtual void OnLoad();
        virtual void OnDestroy();
        virtual void OnUpdate();
        virtual void OnLateUpdate();
        virtual void OnRender();
        virtual void OnGUI();

    private:
        GLFWwindow *window;
        ApplicationSettings settings;
        static void PumpEvents();
        static void FrameBufferSizeCallback(GLFWwindow *window, int32_t width, int32_t height);
        static void KeyPressCallback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
        static void CharPressCallback(GLFWwindow *window, uint32_t codepoint);
        static void MouseButtonPressCallback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods);
        static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    };
}