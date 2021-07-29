#ifndef _ALEC_MODEL_H_
#define _ALEC_MODEL_H_
#include "Model.h"

class AlecModel : public Model
{
public:
	AlecModel()
	{
		for (auto& transform : m_cubeArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}

		for (auto& transform : m_wallArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}
	}

	void OnStart()
	{
		//Model
		//set up model by arranging individual cube's transforms
		m_cubeArr[0]->position = { 0,  0,  0 };
		m_cubeArr[1]->position = { -1,  0,  0 };
		m_cubeArr[2]->position = { -2, 0,  0 };
		m_cubeArr[3]->position = { -1,  -1,  0 };
		m_cubeArr[4]->position = { 0,  1,  0 };
		m_cubeArr[5]->position = { 1,  0,  0 };
		m_cubeArr[6]->position = { 1,  1,  0 };
		m_cubeArr[7]->position = { 2, 0,  0 };
		m_cubeArr[8]->position = { 2, 1,  0 };
		m_cubeArr[9]->position = { 2, -1,  0 };
		m_cubeArr[10]->position = { 0,  0, -1 };
		m_cubeArr[11]->position = { -1, -1, -1 };
		m_cubeArr[12]->position = { 0,  1, -1 };
		m_cubeArr[13]->position = { 2, -1, -1 };

		//then move them all up above the xz plane
		for (auto& transform : m_cubeArr)
		{
			transform->position.y += 3;
		}

		//Wall
		//left side
		m_wallArr[0]->position = { -3, -2, -4 };
		m_wallArr[1]->position = { -3, -1, -4 };
		m_wallArr[2]->position = { -3, 0, -4 };
		m_wallArr[3]->position = { -3, 1, -4 };
		m_wallArr[4]->position = { -3, 2, -4 };

		//bottom
		m_wallArr[5]->position = { -2, -2, -4 };
		m_wallArr[6]->position = { -1, -2, -4 };
		m_wallArr[7]->position = { 0, -2, -4 };
		m_wallArr[8]->position = { 1, -2, -4 };
		m_wallArr[9]->position = { 2, -2, -4 };

		//top
		m_wallArr[10]->position = { -2, 2, -4 };
		m_wallArr[11]->position = { -1, 2, -4 };
		m_wallArr[12]->position = { 0, 2, -4 };
		m_wallArr[13]->position = { 1, 2, -4 };
		m_wallArr[14]->position = { 2, 2, -4 };

		//right
		m_wallArr[15]->position = { 3, -2, -4 };
		m_wallArr[16]->position = { 3, -1, -4 };
		m_wallArr[17]->position = { 3, 0, -4 };
		m_wallArr[18]->position = { 3, 1, -4 };
		m_wallArr[19]->position = { 3, 2, -4 };

		//inside
		m_wallArr[20]->position = { -2, -1, -4 };
		m_wallArr[21]->position = { -2, 1, -4 };
		m_wallArr[22]->position = { -1, 1, -4 };
		m_wallArr[23]->position = { 0, -1, -4 };
		m_wallArr[24]->position = { 1, -1, -4 };


		//then move them all up above the xz plane
		for (auto& transform : m_wallArr)
		{
			transform->position.y += 3;
		}
	}

	void OnRender()
	{
		for (auto& transform : m_cubeArr)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (auto& transform : m_wallArr)
		{
			Renderer3D::DrawVoxel(transform->GetTransformMatrix());
		}
	}


private:
	std::shared_ptr<Transform> m_cubeArr[14];
	std::shared_ptr<Transform> m_wallArr[25];

	glm::vec4 color = { 0.1, 0.9, 0.1, 1 };
};

#endif
