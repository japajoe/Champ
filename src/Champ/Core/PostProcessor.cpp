#include "PostProcessor.hpp"
#include "OpenGL.hpp"

namespace Champ
{
    PostProcessor::PostProcessor()
    {
        
    }

    void PostProcessor::Bind()
    {
        pingpongBuffer->dst->Bind();
    }

    void PostProcessor::Render()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void PostProcessor::SwapBuffers()
    {
        pingpongBuffer->Swap();
    }

    bool PostProcessor::IsActive() const
    {
        return isActive;
    }

    void PostProcessor::SetActive(bool active)
    {
        isActive = active;
    }
}