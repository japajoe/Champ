#include "ApplicationBase.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "OpenGL.hpp"
#include "Graphics.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Champ
{
    ApplicationBase::ApplicationBase(int width, int height, const std::string &title, bool vsync)
    {
        this->width = width;
        this->height = height;
        this->vsync = vsync;
        this->title = title;
        window = nullptr;
    }

    int ApplicationBase::Run()
    {
        if (window)
            return -1;

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return -1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (window == nullptr)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return -1;
        }

        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
        glfwSetKeyCallback(window, KeyPressCallback);
        glfwSetCharCallback(window, CharPressCallback);
        glfwSetMouseButtonCallback(window, MouseButtonPressCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        glfwMakeContextCurrent(window);
        glfwSwapInterval(vsync ? 1 : 0);

        if (!OpenGL::Initialize())
        {
            std::cerr << "Failed to initialize GLAD\n";
            glfwTerminate();
            return -1;
        }

        Graphics::Initialize(width, height, window);
        Input::Initialize(window);

        OnLoad();

        while (!glfwWindowShouldClose(window))
        {
            Time::NewFrame();
            Input::NewFrame();
            OnUpdate();
            OnLateUpdate();
            Graphics::NewFrame();
            OnRender();
            Graphics::EndFrame();
            Graphics::BeginGUI();
            OnGUI();
            Graphics::EndGUI();
            Input::EndFrame();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        OnDestroy();

        Graphics::Destroy();

        glfwTerminate();
        return 0;
    }

    GLFWwindow *ApplicationBase::GetNativeWindow() const
    {
        return window;
    }

    void ApplicationBase::SetWindowLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight)
    {
        glfwSetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight);
    }

    void ApplicationBase::SetTitle(const std::string &title)
    {
        glfwSetWindowTitle(window, title.c_str());
    }

    void ApplicationBase::SetCursor(bool visible)
    {
        glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void ApplicationBase::OnLoad()
    {
    }

    void ApplicationBase::OnDestroy()
    {
    }

    void ApplicationBase::OnUpdate()
    {
    }

    void ApplicationBase::OnLateUpdate()
    {
    }

    void ApplicationBase::OnRender()
    {
    }

    void ApplicationBase::OnPostRender(PingPongBuffer *pingpongBuffer)
    {
    }

    void ApplicationBase::OnGUI()
    {
    }

    void ApplicationBase::FrameBufferSizeCallback(GLFWwindow *window, int32_t width, int32_t height)
    {
        ApplicationBase *app = static_cast<ApplicationBase *>(glfwGetWindowUserPointer(window));
        app->width = width;
        app->height = height;
        Graphics::SetViewport(0, 0, width, height);
    }

    void ApplicationBase::KeyPressCallback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        Input::SetKeyState(static_cast<KeyCode>(key), action > 0 ? 1 : 0);
    }

    void ApplicationBase::CharPressCallback(GLFWwindow *window, uint32_t codepoint)
    {
        Input::AddInputCharacter(codepoint);
    }

    void ApplicationBase::MouseButtonPressCallback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods)
    {
        Input::SetButtonState(static_cast<ButtonCode>(button), action > 0 ? 1 : 0);
    }

    void ApplicationBase::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Input::SetScrollDirection(xoffset, yoffset);
    }
}