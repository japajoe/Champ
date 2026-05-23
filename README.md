# Champ
Small graphics library with all dependencies included. What does it actually do?
- Sets up a window using GLFW, with an OpenGL context (using glad).
- Implements a simple rendering pipeline that renders to a screen texture, which can be used for post processing.
- Initializes ImGui for ease of debugging.
- Input handling for keyboard and mouse.
- Load images using stb_image.
- Load textures and shaders.

The library does nothing more than this. You are expected to have an understanding of OpenGL and manage state yourself. My goal was to do the work required in order to get things to render on screen. There are no fancy abstractions because that's the fun part I want to experiment with.

# Cloning
`git clone --recurse-submodules https://github.com/japajoe/Champ`

# Example
```cpp
#include <Champ/Core/ApplicationBase.hpp>
#include <Champ/Core/OpenGL.hpp>
#include <imgui/imgui.h>
#include <iostream>

using namespace Champ;

class Application : public ApplicationBase
{
public:
    Application(int width, int height, const std::string &title, bool vsync);
protected:
    void OnLoad() override
    {
        std::cout << "Application loaded\n";
    }

    void OnRender() override
    {
        glClearColor(0.01, 0.01, 0.01, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OnGUI() override
    {
        ImGui::Begin("Hello world");
        ImGui::End();
    }
};

int main(int argc, char **argv)
{
    Application app(800, 600, "Champ Demo", true);
    return app.Run();
}
```