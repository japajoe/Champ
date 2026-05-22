# Champ
Simple graphics library with all dependencies included.

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