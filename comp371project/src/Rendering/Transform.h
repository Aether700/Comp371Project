#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

//transform class which contains positional, rotational and scale data of an object
class Transform
{
public:
	glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = glm::vec3{ 0.0f, 0.0f, 0.0f }; //in radians
	glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

	Transform() { }
	Transform(const Transform&) = default;
	Transform(const glm::vec3& position) : position(position) { }
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) 
		: position(position), rotation(rotation), scale(scale) { }

	/*builds the transform matrix of this transform object from the position, rotation and 
	  scale vectors of this transform. will also apply the parent's transform matrix if 
	  this object has a parent transform
	*/
	glm::mat4 GetTransformMatrix() const
	{
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);

		if (m_parent != nullptr)
		{
			return m_parent->GetTransformMatrix() * transform;
		}

		return transform;
	}

	//setter functions which allows the changing and removing of 
	//the parent transform (set parent to nullptr to remove it)
	void SetParent(const std::shared_ptr<Transform>& parent)
	{
		m_parent = parent;
	}

	void SetParent(std::nullptr_t n)
	{
		m_parent = nullptr;
	}

private:
	std::shared_ptr<Transform> m_parent = nullptr;
};

#endif
