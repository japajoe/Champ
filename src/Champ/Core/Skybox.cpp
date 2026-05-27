#include "Skybox.hpp"
#include "OpenGL.hpp"
#include "Time.hpp"
#include "../Shaders/SkyboxShader.hpp"
#include <cstring>

namespace Champ
{
	static Vector3 FromSphericalCoordinates(float radius, float phi, float theta)
	{
        const float sinPhiRadius = glm::sin( phi ) * radius;
		float x = sinPhiRadius * glm::sin( theta );
		float y = glm::cos( phi ) * radius;
		float z = sinPhiRadius * glm::cos( theta );
		return Vector3(x, y, z);
	}

    Skybox::Skybox()
    {
		const float elevation = 2.0f;
		const float azimuth = 180.0f;
		const float phi = glm::radians(90.0f - elevation);
		const float theta = glm::radians(azimuth);

		settings.sunPosition = FromSphericalCoordinates(1.0f, phi, theta);
		settings.rayleigh = 3.0f;
		settings.turbidity = 10.0f;
		settings.mieCoefficient = 0.005f;
		settings.mieDirectionalG = 0.1f;
		settings.cloudScale = 0.0002;
		settings.cloudSpeed = 0.00005;
		settings.cloudCoverage = 0.4;
		settings.cloudDensity = 0.4;
		settings.cloudElevation = 0.5;
		settings.exposure = 1.0f;
    }

    Skybox::Skybox(Skybox &&other) noexcept
    {
        if(this != &other)
        {
            mesh = std::move(other.mesh);
            shader = std::move(other.shader);
            std::memcpy(&settings, &other.settings, sizeof(SkyboxSettings));
        }
    }

    Skybox &Skybox::operator=(Skybox &&other) noexcept
    {
        if(this != &other)
        {
            mesh = std::move(other.mesh);
            shader = std::move(other.shader);
            std::memcpy(&settings, &other.settings, sizeof(SkyboxSettings));
        }
        return *this;
    }

    void Skybox::Generate()
    {
        if(mesh.GetVAO() > 0)
            return;

        mesh = MeshGenerator::CreateCube(Vector3(1, 1, 1));
        shader.Generate(SkyboxShader::GetVertexSource(), SkyboxShader::GetFragmentSource());
    }

    void Skybox::Render(const Matrix4 &view, const Matrix4 &projection)
    {
        if(mesh.GetVAO() == 0)
            return;

        shader.Use();

        const Matrix4 model = glm::identity<Matrix4>();

        shader.SetMat4("uModel", glm::value_ptr(model));
        shader.SetMat4("uView", glm::value_ptr(view));
        shader.SetMat4("uProjection", glm::value_ptr(projection));
        shader.SetFloat3("uSunPosition", glm::value_ptr(settings.sunPosition));
        shader.SetFloat("uRayleigh", settings.rayleigh);
        shader.SetFloat("uTurbidity", settings.turbidity);
        shader.SetFloat("uMieCoefficient", settings.mieCoefficient);
        shader.SetFloat("uMieDirectionalG", settings.mieDirectionalG);
        shader.SetFloat("uCloudScale", settings.cloudScale);
        shader.SetFloat("uCloudSpeed", settings.cloudSpeed);
        shader.SetFloat("uCloudCoverage", settings.cloudCoverage);
        shader.SetFloat("uCloudDensity", settings.cloudDensity);
        shader.SetFloat("uCloudElevation", settings.cloudElevation);
        shader.SetFloat("uExposure", settings.exposure);
        shader.SetFloat("uTime", Time::GetElapsed());

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

		glBindVertexArray(mesh.GetVAO());

		glDrawElements(GL_TRIANGLES, mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    SkyboxSettings *Skybox::GetSettings()
    {
        return &settings;
    }

	Vector3 Skybox::GetSunPositionFromDirection(const Vector3 &direction)
	{
		// Calculate elevation (phi) and azimuth (theta)
		float elevation = glm::degrees(asin(direction.y)); // Elevation in degrees
		float azimuth = glm::degrees(atan2(direction.z, direction.x)); // Azimuth in degrees

		// Optionally, n adjust the azimuth to be in the range [0, 360)
		if (azimuth < 0.0f)
			azimuth += 360.0f;

		const float phi = glm::radians(90.0f - elevation);
		const float theta = glm::radians(90.0f - azimuth);
		
        return FromSphericalCoordinates(1.0f, phi, theta);
	}
}