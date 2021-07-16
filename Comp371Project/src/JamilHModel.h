#pragma once
#include "Model.h"

class JamilHModel : public Model
{
public:
	JamilHModel()
	{
		for (auto& transform : m_cubeArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}
	}

	void OnStart()
	{
		//set up model by arranging individual cube's transforms
		m_cubeArr[0]->position = { 0,  0,  0 };
		m_cubeArr[1]->position = { 0,  1,  0 };
		m_cubeArr[2]->position = { 0, -1,  0 };
		m_cubeArr[3]->position = { 1,  0,  0 };
		m_cubeArr[4]->position = { -1,  0,  0 };
		m_cubeArr[5]->position = { -1,  1,  0 };
		m_cubeArr[6]->position = { -1,  2,  0 };
		m_cubeArr[7]->position = { -1, -1,  0 };
		m_cubeArr[8]->position = { 1, -1,  0 };
		m_cubeArr[9]->position = { 2, -1,  0 };
		m_cubeArr[10]->position = { 0,  0, -1 };
		m_cubeArr[11]->position = { 2, -1, -1 };
		m_cubeArr[12]->position = { -1,  2, -1 };
		m_cubeArr[13]->position = { -1, -1, -1 };
	}

	void OnRender()
	{
		for (auto& transform : m_cubeArr)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}
	}


private:
	std::shared_ptr<Transform> m_cubeArr[14];

	glm::vec4 color = { 1, 0, 0, 1 };
};