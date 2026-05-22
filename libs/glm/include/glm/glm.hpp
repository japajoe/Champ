#ifndef GLM_GLM_HPP
#define GLM_GLM_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include "glmhpp.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_inverse.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/random.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/transform.hpp"
#include "gtx/compatibility.hpp"
#include "gtx/euler_angles.hpp"
#include "gtx/matrix_decompose.hpp"
#include "gtx/rotate_vector.hpp"
#include <string>

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;
using Quaternion = glm::quat;
using Matrix3 = glm::mat3;
using Matrix4 = glm::mat4;

namespace glm
{
	inline glm::vec3 extract_translation(const glm::mat4 &m)
	{
		glm::vec4 position = glm::vec4(m[3]);
		return glm::vec3(position.x, position.y, position.z);
	}

	inline glm::vec3 extract_translation_from_view_matrix(const glm::mat4 &m)
	{
		glm::mat4 inverseViewMatrix = glm::inverse(m);
		glm::vec4 position = inverseViewMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return glm::vec3(position);
	}

	inline glm::quat extract_rotation(const glm::mat4 &m)
	{
		glm::vec3 scale = glm::vec3(glm::length(glm::vec3(m[0])),
				         glm::length(glm::vec3(m[1])),
						 glm::length(glm::vec3(m[2])));

		glm::mat3 rotationMatrix;
		rotationMatrix[0] = glm::vec3(m[0]) / scale.x;
		rotationMatrix[1] = glm::vec3(m[1]) / scale.y;
		rotationMatrix[2] = glm::vec3(m[2]) / scale.z;

		return glm::quat_cast(rotationMatrix);
	}

	inline glm::vec3 extract_scale(const glm::mat4 &m)
	{
		return glm::vec3(glm::length(glm::vec3(m[0])),
				         glm::length(glm::vec3(m[1])),
						 glm::length(glm::vec3(m[2])));
	}

	inline glm::vec3 project_on_plane(const glm::vec3 &vector, const glm::vec3 &planeNormal)
	{
        float dot = glm::dot(vector, planeNormal);
        
        float magSq = glm::dot(planeNormal, planeNormal);
        
        if (magSq < 0.000001f)
            return vector;

        // Subtract the projection of the vector onto the normal from the original vector
        // Formula: v - (n * (v . n) / |n|^2)
        return vector - (planeNormal * dot / magSq);
	}

	inline glm::vec3 random_on_unit_sphere()
	{
		return glm::sphericalRand(1.0f);
	}

	inline glm::vec2 random_inside_unit_circle()
	{
		return glm::diskRand(1.0f);
	}

	inline float angle(const glm::vec3 &from, const glm::vec3 &to)
	{
        float magProduct = glm::sqrt(glm::dot(from, from) * glm::dot(to, to));

        if (magProduct < 0.000001f)
            return 0.0f;

        // Clamp the dot product result to [-1, 1] to avoid NaN from floating point errors in acos
        float dot = glm::clamp(glm::dot(from, to) / magProduct, -1.0f, 1.0f);

		// Calculate arc cosine and convert from radians to degrees
        return glm::acos(dot) * (180.0f / glm::pi<float>());
	}

	inline glm::mat4 clear_rotation(const glm::mat4 &m)
	{
		glm::mat4 result = m;
		// 1. Get the current scale (length of the basis vectors)
		float scaleX = glm::length(glm::vec3(result[0]));
		float scaleY = glm::length(glm::vec3(result[1]));
		float scaleZ = glm::length(glm::vec3(result[2]));

		// 2. Reset the columns but multiply by the original scale
		result[0] = glm::vec4(scaleX, 0, 0, 0);
		result[1] = glm::vec4(0, scaleY, 0, 0);
		result[2] = glm::vec4(0, 0, scaleZ, 0);

		return result;
	}

	inline glm::quat look_rotation(const glm::vec3 &direction)
	{
		glm::vec3 forward = glm::normalize(direction);
		// Flatten the direction vector on the Y axis
		forward.y = 0.0f;

		if (glm::length(forward) < 0.001f)
		{
			return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		}

		forward = glm::normalize(forward);

		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		// Calculate Right and Up vectors
		// To account for -Z forward, we invert the cross product logic 
		// to align the local -Z with the target direction
		glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
		glm::vec3 up = glm::cross(right, forward);

		glm::mat3 rotationMatrix;
		// Map the local axes to the world-space directions
		rotationMatrix[0] = right;       // Local X
		rotationMatrix[1] = up;          // Local Y
		rotationMatrix[2] = -forward;    // Local Z (Inverted to point -Z forward)

		glm::quat targetRotation = glm::quat_cast(rotationMatrix);

		return targetRotation;
	}

	inline glm::vec3 transform(const glm::vec3 &v, const glm::quat &r)
	{
		const float x = r.x + r.x;
        const float y = r.y + r.y;
        const float z = r.z + r.z;
        const float wx = r.w * x;
        const float wy = r.w * y;
        const float wz = r.w * z;
        const float xx = r.x * x;
        const float xy = r.x * y;
        const float xz = r.x * z;
        const float yy = r.y * y;
        const float yz = r.y * z;
        const float zz = r.z * z;
        return glm::vec3(
            v.x * (1.0f - yy - zz) + v.y * (xy - wz) + v.z * (xz + wy),
            v.x * (xy + wz) + v.y * (1.0f - xx - zz) + v.z * (yz - wx),
            v.x * (xz - wy) + v.y * (yz + wx) + v.z * (1.0f - xx - yy));
	}

	inline float inverse_lerp(float start, float end, float value)
	{
		return (value - start) / (end - start);
	}

	inline std::string to_string(const glm::vec3 &v)
	{
		return  std::to_string(v.x) + ", " + 
				std::to_string(v.y) + ", " +
				std::to_string(v.z);
	}
	
	inline std::string to_string(const glm::vec2 &v)
	{
		return  std::to_string(v.x) + ", " + 
				std::to_string(v.y);
	}	
}

#endif