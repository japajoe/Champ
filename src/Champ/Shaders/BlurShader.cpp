#include "BlurShader.hpp"
#include "ScreenShader.hpp"

namespace Champ
{
    static std::string gFragmentSource = R"(in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uHorizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    // Explicitly cast ivec2 to vec2 for the division
    vec2 texOffset = 1.0 / vec2(textureSize(uTexture, 0)); 
    vec3 result = texture(uTexture, TexCoords).rgb * weight[0]; 
    
    if(uHorizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(uTexture, TexCoords + vec2(texOffset.x * float(i), 0.0)).rgb * weight[i];
            result += texture(uTexture, TexCoords - vec2(texOffset.x * float(i), 0.0)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(uTexture, TexCoords + vec2(0.0, texOffset.y * float(i))).rgb * weight[i];
            result += texture(uTexture, TexCoords - vec2(0.0, texOffset.y * float(i))).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
})";

    std::string BlurShader::GetVertexSource()
    {
        return ScreenShader::GetVertexSource();
    }

    std::string BlurShader::GetFragmentSource()
    {
        return gFragmentSource;
    }

}

