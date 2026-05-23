#include "VHSShader.hpp"

namespace Champ
{
    static std::string gVertexSource = R"(#version 330 core
out vec2 TexCoords;

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

    static std::string gFragmentSource = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uTexture;
uniform float uTime;

float noise(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec2 uv = TexCoords;
    
    // 1. Horizontal jitter/shaking line distortion
    float jitterTime = uTime * 10.0;
    float drift = sin(uv.y * 10.0 + jitterTime) * 0.003;
    float trackingNoise = step(0.05, noise(vec2(uTime, uv.y))) * 0.005;
    
    if (noise(vec2(uTime, uv.y * 20.0)) > 0.98)
    {
        uv.x += drift + trackingNoise;
    }

    // 2. Center-dependent Chromatic Aberration
    vec2 centerDist = uv - vec2(0.5, 0.5);
    float falloff = dot(centerDist, centerDist);

    float maxOffset = 0.006; 
    float rOffset = maxOffset * sin(uTime * 2.0) * falloff;
    float bOffset = -maxOffset * cos(uTime * 2.0) * falloff;
    float gOffset = 0.0;

    float r = texture(uTexture, vec2(uv.x + rOffset, uv.y)).r;
    float g = texture(uTexture, vec2(uv.x + gOffset, uv.y)).g;
    float b = texture(uTexture, vec2(uv.x + bOffset, uv.y)).b;

    vec3 color = vec3(r, g, b);

    // 3. Scanlines
    float scanline = sin(uv.y * 800.0) * 0.004;
    color -= scanline;

    // 4. Static / Grain Noise
    float grain = (noise(uv + uTime) - 0.5) * 0.025;
    color += grain;

    // 5. Tape Warp / Vignette at edges
    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(16.0 * vignette, 0.25), 0.0, 1.0);
    color *= vignette;

    FragColor = vec4(color, 1.0);
})";

    std::string VHSShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string VHSShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}