#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Champ
{
	struct ShaderSource
	{
		std::string fragment;
		std::string geometry;
		std::string vertex;
	};

	class Shader
	{
	public:
		Shader();
		Shader(const Shader &other) = delete;
		Shader(Shader &&other) noexcept;
		Shader &operator=(const Shader &other) = delete;
		Shader &operator=(Shader &&other) noexcept;
		void Generate(const std::string &vertexSource, const std::string &fragmentSource);
		void Destroy();
		void Use();
		uint32_t GetId() const;
		void SetInt(const char *name, int32_t value);
		void SetFloat(const char *name, float value);
		void SetFloat2(const char *name, const float *value);
		void SetFloat3(const char *name, const float *value);
		void SetFloat4(const char *name, const float *value);
		void SetMat2(const char *name, const float *value);
		void SetMat3(const char *name, const float *value);
		void SetMat4(const char *name, const float *value);
		void SetIntEx(int32_t location, int32_t value);
		void SetUIntEx(int32_t location, uint32_t value);
		void SetFloatEx(int32_t location, float value);
		void SetFloat2Ex(int32_t location, const float *value);
		void SetFloat3Ex(int32_t location, const float *value);
		void SetFloat4Ex(int32_t location, const float *value);
		void SetMat2Ex(int32_t location, const float *value);
		void SetMat3Ex(int32_t location, const float *value);
		void SetMat4Ex(int32_t location, const float *value);
		static ShaderSource GetScreenShaderSource();

	private:
		uint32_t m_id;
	};
}