#pragma once
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../Dependencies/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"

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


	glm::mat4 GetTransformMatrix() const
	{
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

		return glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
	}
};