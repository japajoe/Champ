#pragma once

#include <cstdint>
#include <vector>
#include "OpenGL.hpp"

namespace Champ
{
    class UniformBuffer
    {
    public:
        UniformBuffer();
        UniformBuffer(const UniformBuffer &other) = delete;
        UniformBuffer(UniformBuffer &&other) noexcept;
        UniformBuffer& operator=(const UniformBuffer &other) = delete;
        UniformBuffer& operator=(UniformBuffer &&other) noexcept;
        void Generate();
        void Destroy();
        void Bind();
        void Unbind();
        void BindBufferBase(GLuint index);
        void BufferData(GLsizeiptr size, const void *data, GLenum usage);
        void BufferSubData(GLintptr offset, GLsizeiptr size, const void *data);
        uint32_t GetUniformBlockIndex(GLuint shaderProgram, const char *uniformBlockName);
        void UniformBlockBinding(GLuint shaderProgram, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
        void BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const char *blockName);
        uint32_t GetId() const;
        
        template<typename T>
        static UniformBuffer Create(GLuint bindingIndex, GLuint numItems)
        {
            UniformBuffer ubo;
            ubo.Generate();
            ubo.Bind();

            std::vector<T> data;
			data.resize(numItems);
            
            ubo.BufferData(data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
            ubo.BindBufferBase(bindingIndex);
            ubo.Unbind();

            return ubo;
        }

        static UniformBuffer Create(GLuint bindingIndex, GLuint size)
        {
            UniformBuffer ubo;
            ubo.Generate();
            ubo.Bind();

            std::vector<uint8_t> data;
			data.resize(size);
            
            ubo.BufferData(data.size(), data.data(), GL_DYNAMIC_DRAW);
            ubo.BindBufferBase(bindingIndex);
            ubo.Unbind();

            return ubo;
        }

    private:
        uint32_t m_id;
    };
}