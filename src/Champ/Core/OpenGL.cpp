#include "OpenGL.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

namespace Champ
{
    static float gMaxAnisotropy = 0.0f;
    static int32_t gMaxTextureAttachments = 4;
    static int32_t gMaxDrawBuffers = 4;

    bool OpenGL::Initialize()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            return false;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gMaxAnisotropy);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gMaxTextureAttachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &gMaxDrawBuffers);

        return true;
    }

    float OpenGL::GetMaxAnisotropyLevel()
    {
        return gMaxAnisotropy;
    }

    GLint OpenGL::GetMaxTextureAttachments()
    {
        return gMaxTextureAttachments;
    }

    GLint OpenGL::GetMaxDrawBuffers()
    {
        return gMaxDrawBuffers;
    }
}