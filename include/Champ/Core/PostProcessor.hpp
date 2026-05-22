#pragma once

#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Champ
{
    class PostProcessor
    {
        friend class Graphics;

    public:
        PostProcessor();
        virtual void Initialize() = 0;
        virtual void OnProcess(const Matrix4 &projection, const Matrix4 &view) = 0;
        bool IsActive() const;
        void SetActive(bool active);

    protected:
        void Bind();
        void Render();
        void SwapBuffers();

    private:
        uint32_t vao;
        Shader *shader;
        PingPongBuffer *pingpongBuffer;
        bool isActive = true;
    };
}