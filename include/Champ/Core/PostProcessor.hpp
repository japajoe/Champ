#pragma once

#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Champ
{
    /// @brief Base class for post processing effects. Typically you call `Bind`, then `shader->Use`, followed by `Render` and `SwapBuffers`.
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
        Shader *shader;
        PingPongBuffer *pingpongBuffer;
        bool isActive = true;
        void Bind();
        void Render();
        void SwapBuffers();
        uint32_t GetInputTexture() const;
    private:
        uint32_t vao;
    };
}