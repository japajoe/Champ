#include "Transform.hpp"
#include "Time.hpp"
#include <algorithm>
#include <stack>
#include <utility>

namespace Champ
{
	Transform::Transform()
	{
		m_localPosition = Vector3(0, 0, 0);
		m_localRotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
		m_localScale = Vector3(1, 1, 1);
		m_velocity = Vector3(0, 0, 0);
		m_previousPosition = Vector3(0, 0, 0);
		m_eulerRotation = Vector3(0, 0, 0);
		m_parent = nullptr;
		m_root = this;
		m_isDirty = true;
		m_lastFrameCount = std::numeric_limits<uint64_t>::max();
	}

	Transform::~Transform()
	{
		if (m_parent)
		{
			m_parent->RemoveChild(this);
		}

		for (Transform *child : m_children)
		{
			child->SetParent(nullptr);
		}
	}

	Transform::Transform(const Transform &other)
	{
		m_localPosition = other.m_localPosition;
		m_localScale = other.m_localScale;
		m_localRotation = other.m_localRotation;
		m_previousPosition = other.m_previousPosition;
		m_velocity = other.m_velocity;
		m_eulerRotation = other.m_eulerRotation;
		m_parent = other.m_parent;
		m_root = other.m_root;
		m_children = other.m_children;
		m_isDirty = other.m_isDirty;
		m_lastFrameCount = other.m_lastFrameCount;
	}

	Transform::Transform(Transform &&other) noexcept
	{
		m_localPosition = std::exchange(other.m_localPosition, Vector3(0, 0, 0));
		m_localScale = std::exchange(other.m_localScale, Vector3(1, 1, 1));
		m_localRotation = std::exchange(other.m_localRotation, Quaternion(1, 0, 0, 0));
		m_previousPosition = std::exchange(other.m_previousPosition, Vector3(0, 0, 0));
		m_velocity = std::exchange(other.m_velocity, Vector3(0, 0, 0));
		m_eulerRotation = std::exchange(other.m_eulerRotation, Vector3(0, 0, 0));
		m_parent = std::exchange(other.m_parent, nullptr);
		m_root = std::exchange(other.m_root, &other);
		m_children = std::move(other.m_children);
		m_isDirty = std::exchange(other.m_isDirty, false);
		m_lastFrameCount = other.m_lastFrameCount;
	}

	Transform &Transform::operator=(const Transform &other)
	{
		if (this != &other)
		{
			m_localPosition = other.m_localPosition;
			m_localScale = other.m_localScale;
			m_localRotation = other.m_localRotation;
			m_previousPosition = other.m_previousPosition;
			m_velocity = other.m_velocity;
			m_eulerRotation = other.m_eulerRotation;
			m_parent = other.m_parent;
			m_root = other.m_root;
			m_children = other.m_children;
			m_isDirty = other.m_isDirty;
			m_lastFrameCount = other.m_lastFrameCount;
		}
		return *this;
	}

	Transform &Transform::operator=(Transform &&other) noexcept
	{
		if (this != &other)
		{
			m_localPosition = std::exchange(other.m_localPosition, Vector3(0, 0, 0));
			m_localScale = std::exchange(other.m_localScale, Vector3(1, 1, 1));
			m_localRotation = std::exchange(other.m_localRotation, Quaternion(1, 0, 0, 0));
			m_previousPosition = std::exchange(other.m_previousPosition, Vector3(0, 0, 0));
			m_velocity = std::exchange(other.m_velocity, Vector3(0, 0, 0));
			m_eulerRotation = std::exchange(other.m_eulerRotation, Vector3(0, 0, 0));
			m_parent = std::exchange(other.m_parent, nullptr);
			m_root = std::exchange(other.m_root, &other);
			m_children = std::move(other.m_children);
			m_isDirty = std::exchange(other.m_isDirty, false);
			m_lastFrameCount = other.m_lastFrameCount;
		}
		return *this;
	}

	void Transform::SetPosition(const Vector3 &position)
	{
		if (m_parent)
		{
			Matrix4 parentMatrix = m_parent->GetModelMatrix();
			Matrix4 invParentMatrix = glm::inverse(parentMatrix);
			m_localPosition = glm::vec3(invParentMatrix * glm::vec4(position, 1.0f));
		}
		else
		{
			m_localPosition = position;
		}

		MarkDirty();
	}

	Vector3 Transform::GetPosition() const
	{
		return glm::vec3(GetModelMatrix()[3]);
	}

	void Transform::SetLocalPosition(const Vector3 &position)
	{
		m_localPosition = position;
		MarkDirty();
	}

	Vector3 Transform::GetLocalPosition() const
	{
		return m_localPosition;
	}

	void Transform::SetRotation(const Quaternion &rotation)
	{
		if (m_parent)
		{
			m_localRotation = glm::inverse(m_parent->GetRotation()) * rotation;
		}
		else
		{
			m_localRotation = rotation;
		}

		MarkDirty();
	}

	Quaternion Transform::GetRotation() const
	{
		if (m_parent)
		{
			return m_parent->GetRotation() * m_localRotation;
		}
		else
		{
			return m_localRotation;
		}
	}

	void Transform::SetLocalRotation(const Quaternion &rotation)
	{
		m_localRotation = rotation;
		MarkDirty();
	}

	Quaternion Transform::GetLocalRotation() const
	{
		return m_localRotation;
	}

	void Transform::SetScale(const Vector3 &scale)
	{
		m_localScale = scale;
		MarkDirty();
	}

	Vector3 Transform::GetScale() const
	{
		return m_localScale;
	}

	Vector3 Transform::GetForward() const
	{
		return GetRotation() * Vector3(0.0f, 0.0f, -1.0f);
	}

	Vector3 Transform::GetRight() const
	{
		return GetRotation() * Vector3(1.0f, 0.0f, 0.0f);
	}

	Vector3 Transform::GetUp() const
	{
		return GetRotation() * Vector3(0.0f, 1.0f, 0.0f);
	}

	Vector3 Transform::GetVelocity()
	{
		if (m_lastFrameCount != Time::GetFrameCount())
		{
			float deltaTime = Time::GetDeltaTime();
			Vector3 currentPosition = GetPosition();

			if (deltaTime > 0.0001f)
				m_velocity = (currentPosition - m_previousPosition) / deltaTime;
			else
				m_velocity = Vector3(0, 0, 0);

			m_previousPosition = currentPosition;
			m_lastFrameCount = Time::GetFrameCount();
		}

		return m_velocity;
	}

	Matrix4 Transform::GetModelMatrix() const
	{
		if (m_isDirty)
		{
			RecalculateModelMatrix();
		}
		return m_cachedModelMatrix;
	}

	void Transform::SetParent(Transform *newParent)
	{
		// Prevent being able to set the same parent as the current
		if (m_parent == newParent)
			return;

		// Can't set parent to itself
		if (newParent == this)
			return;

		// Save current world Transform
		Vector3 worldPos = GetPosition();
		Quaternion worldRot = GetRotation();

		// Remove from old parent if any
		if (m_parent)
		{
			m_parent->RemoveChild(this);
		}

		// Assign new parent
		m_parent = newParent;

		// Add to new parent's children if not null
		if (m_parent)
		{
			m_parent->AddChild(this);
		}

		// Preserve world Transform
		SetPosition(worldPos);
		SetRotation(worldRot);

		// Recalculate root
		m_root = this;
		Transform *current = m_parent;
		while (current != nullptr)
		{
			m_root = current;
			current = current->m_parent;
		}

		MarkDirty();
	}

	Transform *Transform::GetParent() const
	{
		return m_parent;
	}

	Transform *Transform::GetRoot() const
	{
		return m_root;
	}

	const std::vector<Transform *> &Transform::GetChildren() const
	{
		return m_children;
	}

	void Transform::MarkDirty()
	{
		if (m_isDirty)
			return;

		m_isDirty = true;
		std::stack<Transform *> stack;
		stack.push(this);

		while (!stack.empty())
		{
			Transform *current = stack.top();
			stack.pop();

			for (Transform *child : current->m_children)
			{
				if (!child->m_isDirty)
				{
					child->m_isDirty = true;
					stack.push(child);
				}
			}
		}
	}

	void Transform::RecalculateModelMatrix() const
	{
		Matrix4 localMatrix = glm::translate(glm::mat4(1.0f), m_localPosition) * glm::toMat4(m_localRotation) * glm::scale(glm::mat4(1.0f), m_localScale);

		if (m_parent)
		{
			m_cachedModelMatrix = m_parent->GetModelMatrix() * localMatrix;
		}
		else
		{
			m_cachedModelMatrix = localMatrix;
		}

		m_isDirty = false;
	}

	void Transform::AddChild(Transform *child)
	{
		m_children.push_back(child);
	}

	void Transform::RemoveChild(Transform *child)
	{
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			m_children.erase(it);
		}
	}

	void Transform::LookAt(const Vector3 &target, const Vector3 &up)
	{
		const auto pos = GetPosition();
		const auto mat = glm::lookAt(pos, target, up);
		const auto rot = glm::quat_cast(Matrix3(mat));
		Rotate(rot);
	}

	void Transform::Translate(const Vector3 &translation)
	{
		SetPosition(GetPosition() + translation);
	}

	void Transform::Rotate(const Vector3 &rotation)
	{
		const auto currentRotation = glm::eulerAngles(GetRotation());

		m_eulerRotation.x += rotation.x;
		m_eulerRotation.y += rotation.y;
		m_eulerRotation.z += rotation.z;

		const auto rotationDelta = m_eulerRotation - currentRotation;

		auto rott = GetRotation();

		rott = rott * glm::angleAxis(glm::radians(rotationDelta.x), Vector3(1, 0, 0));
		rott = rott * glm::angleAxis(glm::radians(rotationDelta.y), Vector3(0, 1, 0));
		rott = rott * glm::angleAxis(glm::radians(-rotationDelta.z), Vector3(0, 0, -1));

		m_eulerRotation = glm::eulerAngles(rott);
		SetRotation(rott);
	}

	void Transform::Rotate(const Quaternion &rotation)
	{
		SetRotation(rotation);
	}

	Vector3 Transform::TransformDirection(const Vector3 &direction) const
	{
		return GetRotation() * direction;
	}

	Vector3 Transform::InverseTransformDirection(const Vector3 &direction) const
	{
		Vector3 dir = Matrix3(glm::inverse(GetModelMatrix())) * direction;
		return glm::normalize(dir);
	}

	Vector3 Transform::InverseTransformPoint(const Vector3 &point) const
	{
		// Step 1: Translate the point to the origin
		Vector3 pointRelativeToOrigin = point - GetPosition();

		// Step 2: Rotate the point back using the inverse of the rotation
		Quaternion inverseRotation = glm::inverse(GetRotation());
		Vector3 localPoint = inverseRotation * pointRelativeToOrigin;

		// Step 3: Scale the point back (if needed)
		// Note: Scaling is usually applied in the forward Transform, so we may not need to apply it here.
		// If you want to consider scaling, you can divide by the scale factors.
		Vector3 scale = GetScale();
		localPoint.x /= scale.x;
		localPoint.y /= scale.y;
		localPoint.z /= scale.z;

		return localPoint;
	}

	Matrix4 Transform::CreateModelMatrix(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
	{
		return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
	}

	Vector3 Transform::WorldToLocal(const Vector3 &v)
	{
		auto invScale = GetScale();
		if (invScale.x != 0.0f)
			invScale.x = 1.0f / invScale.x;
		if (invScale.y != 0.0f)
			invScale.y = 1.0f / invScale.y;
		if (invScale.z != 0.0f)
			invScale.z = 1.0f / invScale.z;

		const Quaternion invRotation = glm::conjugate(GetRotation());
		Vector3 result = v - GetPosition();
		result = glm::transform(result, invRotation);
		result *= invScale;
		return result;
	}

	Vector3 Transform::WorldToLocalVector(const Vector3 &v)
	{
		auto invScale = GetScale();
		if (invScale.x != 0.0f)
			invScale.x = 1.0f / invScale.x;
		if (invScale.y != 0.0f)
			invScale.y = 1.0f / invScale.y;
		if (invScale.z != 0.0f)
			invScale.z = 1.0f / invScale.z;
		const Quaternion invRotation = glm::conjugate(GetRotation());
		Vector3 result = glm::transform(v, invRotation);
		result *= invScale;
		return result;
	}

	Vector3 Transform::LocalToWorld(const Vector3 &v)
	{
		Vector3 tmp = v * GetScale();
		tmp = glm::transform(tmp, GetRotation());
		return tmp + GetPosition();
	}

	Vector3 Transform::LocalToWorldVector(const Vector3 &v)
	{
		Vector3 tmp = v * GetScale();
		return glm::transform(tmp, GetRotation());
	}
}