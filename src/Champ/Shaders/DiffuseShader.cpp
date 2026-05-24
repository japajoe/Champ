#include "DiffuseShader.hpp"

namespace Champ
{
    static std::string gVertexSource = R"(layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uMVP;
uniform mat3 uModelInverted;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    vec4 clipPosition = uMVP * vec4(aPos, 1.0);
    vec4 worldPos = uModel * vec4(aPos, 1.0);

    FragPos = worldPos.xyz;
    //glm::mat3 uModelInverted = glm::inverse(glm::transpose(glm::mat3(model)));
    Normal = normalize(uModelInverted * aNormal);
    TexCoords = aTexCoords;
    gl_Position = clipPosition;
})";

static std::string gFragmentSource = R"(layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightnessColor;

uniform sampler2D uTexture;
uniform vec4 uAlbedo;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    vec4 texColor = texture(uTexture, TexCoords);
    vec4 baseColor = texColor * uAlbedo;

    vec4 ambient = 0.2 * baseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * baseColor;

    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec4 specular = spec * vec4(0.5);

    vec4 result = ambient + diffuse + specular;
    FragColor = result;
})";

    std::string DiffuseShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string DiffuseShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}