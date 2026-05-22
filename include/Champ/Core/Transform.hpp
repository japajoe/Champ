#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace Champ
{
	class Transform
	{
	public:
		Transform();
		~Transform();
		Transform(const Transform &other);
		Transform(Transform &&other) noexcept;
		Transform &operator=(const Transform &other);
		Transform &operator=(Transform &&other) noexcept;
		void SetPosition(const Vector3 &position);
		Vector3 GetPosition() const;
		void SetLocalPosition(const Vector3 &position);
		Vector3 GetLocalPosition() const;
		void SetRotation(const Quaternion &rotation);
		Quaternion GetRotation() const;
		void SetLocalRotation(const Quaternion &rotation);
		Quaternion GetLocalRotation() const;
		void SetScale(const Vector3 &scale);
		Vector3 GetScale() const;
		Vector3 GetForward() const;
		Vector3 GetRight() const;
		Vector3 GetUp() const;
		Vector3 GetVelocity();
		Matrix4 GetModelMatrix() const;
		void SetParent(Transform *newParent);
		Transform *GetParent() const;
		Transform *GetRoot() const;
		const std::vector<Transform *> &GetChildren() const;
		void LookAt(const Vector3 &target, const Vector3 &up);
		void Translate(const Vector3 &translation);
		void Rotate(const Vector3 &rotation);
		void Rotate(const Quaternion &rotation);
		Vector3 TransformDirection(const Vector3 &direction) const;
		Vector3 InverseTransformDirection(const Vector3 &direction) const;
		Vector3 InverseTransformPoint(const Vector3 &point) const;
		Vector3 WorldToLocal(const Vector3 &v);
		Vector3 WorldToLocalVector(const Vector3 &v);
		Vector3 LocalToWorld(const Vector3 &v);
		Vector3 LocalToWorldVector(const Vector3 &v);
		static Matrix4 CreateModelMatrix(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale);

	private:
		void MarkDirty();
		void RecalculateModelMatrix() const;
		void AddChild(Transform *child);
		void RemoveChild(Transform *child);
		Vector3 m_localPosition;
		Quaternion m_localRotation;
		Vector3 m_localScale;
		Vector3 m_velocity;
		Vector3 m_previousPosition;
		Vector3 m_eulerRotation;
		Transform *m_parent;
		Transform *m_root;
		uint64_t m_lastFrameCount;
		std::vector<Transform *> m_children;
		mutable Matrix4 m_cachedModelMatrix;
		mutable bool m_isDirty;
	};
}