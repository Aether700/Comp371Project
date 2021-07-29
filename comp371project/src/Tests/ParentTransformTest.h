#ifndef _PARENT_TRANSFoRM_TEST_H_
#define _PARENT_TRANSFoRM_TEST_H_

#include "../Core/Script.h"
#include "../Core/Time.h"
#include "../Rendering/Transform.h"
#include "../Rendering/Renderer3D.h"

class ParentTransformTest : public Script
{
public:
	void OnStart()
	{
		m_parent = std::make_shared<Transform>();
		m_cube1 = std::make_shared<Transform>(glm::vec3{ 3, 0, 0 }, glm::vec3{ glm::radians(45.0f), 
			glm::radians(45.0f), 0.0f }, glm::vec3{1, 1, 1});
		m_cube2 = std::make_shared<Transform>(glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0,
			glm::radians(45.0f) }, glm::vec3{ 1, 1, 1 });

		m_cube1->SetParent(m_parent);
		m_cube2->SetParent(m_parent);
	}

	void OnRender()
	{
		m_currRotation += m_rotationSpeed * Time::GetDeltaTime();
		m_parent->rotation.z = m_currRotation;
		Renderer3D::DrawPointCube(m_cube1->GetTransformMatrix(), {0.85f, 0.0f, 0.5f, 1.0f});
		Renderer3D::DrawWireCube(m_cube2->GetTransformMatrix());
	}

private:

	float m_currRotation = 0.0f;
	float m_rotationSpeed = 0.75f;

	std::shared_ptr<Transform> m_parent;
	std::shared_ptr<Transform> m_cube1;
	std::shared_ptr<Transform> m_cube2;
};

#endif
