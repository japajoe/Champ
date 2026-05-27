#pragma once

#include "Mesh3D.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>

namespace Champ
{
	struct SkyboxSettings
	{
		float rayleigh;
		float turbidity;
		float mieCoefficient;
		float mieDirectionalG;
		float cloudScale;
		float cloudSpeed;
		float cloudCoverage;
		float cloudDensity;
		float cloudElevation;
		float exposure;
		Vector3 sunPosition;
	};

    class Skybox
    {
    public:
        Skybox();
        Skybox(const Skybox &other) = delete;
        Skybox(Skybox &&other) noexcept;
        Skybox &operator=(const Skybox &other) = delete;
        Skybox &operator=(Skybox &&other) noexcept;
        void Generate();
        void Render(const Matrix4 &view, const Matrix4 &projection);
        SkyboxSettings *GetSettings();
        Vector3 GetSunPositionFromDirection(const Vector3 &direction);
    private:
        Mesh3D mesh;
        Shader shader;
        SkyboxSettings settings;
    };
}