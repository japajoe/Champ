#include "ScreenShader.hpp"

namespace Champ
{
    static std::string gVertexSource = R"(out vec2 TexCoords;

void main() {
    // Generates a triangle that covers the [-1, 1] range
    // Vertex 0: (-1, -1), UV (0, 0)
    // Vertex 1: ( 3, -1), UV (2, 0)
    // Vertex 2: (-1,  3), UV (0, 2)
    
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    
    TexCoords.x = (x + 1.0) * 0.5;
    TexCoords.y = (y + 1.0) * 0.5;
    
    gl_Position = vec4(x, y, 0.0, 1.0);
})";

    static std::string gFragmentSource = R"(uniform sampler2D uTexture;

in vec2 TexCoords;
out vec4 FragColor;

vec3 aces_tonemapping(vec3 color) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main() {
	vec3 color = texture(uTexture, TexCoords).rgb;
	color = aces_tonemapping(color);
	color = pow(color.rgb, vec3(0.454545455));
	FragColor = vec4(color, 1.0);
})";

    std::string ScreenShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string ScreenShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}