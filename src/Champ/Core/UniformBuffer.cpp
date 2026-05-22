#include "UniformBuffer.hpp"
#include <utility>

namespace Champ
{
	UniformBuffer::UniformBuffer()
	{
		m_id = 0;
	}
	
	UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
	{
		m_id = std::exchange(other.m_id, 0);
	}
	
	UniformBuffer& UniformBuffer::operator=(UniformBuffer &&other) noexcept
	{
        if(this != &other)
        {
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
	}
	
	void UniformBuffer::Generate()
	{
		glGenBuffers(1, &m_id);
	}
	
	void UniformBuffer::Destroy()
	{
        if(m_id)
            glDeleteBuffers(1, &m_id);
		m_id = 0;
	}
	
	void UniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	}
	
	void UniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	
	void UniformBuffer::BindBufferBase(GLuint index)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, index, m_id);
	}
	
	void UniformBuffer::BufferData(GLsizeiptr size, const void *data, GLenum usage)
	{
		glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
	}
	
	void UniformBuffer::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}
	
	uint32_t UniformBuffer::GetUniformBlockIndex(GLuint shaderProgram, const char *uniformBlockName)
	{
		return glGetUniformBlockIndex(shaderProgram, uniformBlockName);
	}
	
	void UniformBuffer::UniformBlockBinding(GLuint shaderProgram, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		glUniformBlockBinding(shaderProgram, uniformBlockIndex, uniformBlockBinding);
	}
	
	void UniformBuffer::BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const char *blockName)
	{
		GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, blockName);
		glUniformBlockBinding(shaderProgram, blockIndex, bindingIndex);
	}
	
	uint32_t UniformBuffer::GetId() const
	{
		return m_id;
	}
}