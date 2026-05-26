#include "PixelatorShader.hpp"
#include "ScreenShader.hpp"

namespace Champ
{
    static std::string gFragmentSource = R"(uniform sampler2D uTexture;
uniform float uPixelSize;

in vec2 TexCoords;
out vec4 FragColor;

void main() {
    vec2 size = vec2(textureSize(uTexture, 0));
    
    float dx = uPixelSize * (1.0 / size.x);
    float dy = uPixelSize * (1.0 / size.y);
    
    vec2 coord = vec2(dx * floor(TexCoords.x / dx),
                      dy * floor(TexCoords.y / dy));
                      
    FragColor = texture(uTexture, coord);
})";

    std::string PixelatorShader::GetVertexSource()
    {
        return ScreenShader::GetVertexSource();
    }

    std::string PixelatorShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}