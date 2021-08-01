#pragma once
#include "Core/Script.h"
#include "Rendering/Renderer3D.h"

class Axes : public Script
{
public:
	Axes(std::shared_ptr<Transform> worldTransform)
	{
		m_x = std::make_shared<Transform>(glm::vec3(2.5,0,0), glm::vec3(0,glm::radians(0.0f),0), glm::vec3(5,0,0));
		m_y = std::make_shared<Transform>(glm::vec3(0,2.5,0), glm::vec3(0,0,glm::radians(90.0f)), glm::vec3(5, 0, 0));
		m_z = std::make_shared<Transform>(glm::vec3(0,0,2.4), glm::vec3(0, glm::radians(90.0f),0), glm::vec3(5, 0, 0));
		m_x->SetParent(worldTransform);
		m_y->SetParent(worldTransform);
		m_z->SetParent(worldTransform);
		
	}
	//GetTransformMatrix()

	void OnRender()
	{
		//x
		Renderer3D::DrawLine(m_x->GetTransformMatrix(),{ 1,0,0,0 });

		//y
		Renderer3D::DrawLine(m_y->GetTransformMatrix(), { 0,1,0,0 });

		//z
		Renderer3D::DrawLine(m_z->GetTransformMatrix(), { 0,0,1,0 });
	}

private:
	std::shared_ptr<Transform> m_x;
	std::shared_ptr<Transform> m_y;
	std::shared_ptr<Transform> m_z;


};
