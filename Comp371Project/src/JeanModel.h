#pragma once

#include "Model.h"

class JeanModel : public Model
{
public:

	JeanModel()
	{
		for (auto& transform : m_cubeModel)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}

		for (auto& transform : m_wallCubes)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}
	}

	void OnStart()
	{

		SetModelCubesTransform();
		SetWallCubesTransform();
	}

	void OnRender()
	{
		for (auto& transform : m_cubeModel)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (auto& transform : m_wallCubes)
		{
			transform->scale.z = 0.25f;
			RenderCube(transform->GetTransformMatrix());
		}
	}


private:

	std::shared_ptr<Transform> m_cubeModel[16];
	//wall size = roughly model size x2= 5x2 x 2x5 = 100cubes
	std::shared_ptr<Transform>  m_wallCubes[100];
	int modelYOffset = 3;
	int modelXOffset = 3;
	int modelZOffset = 5;


	void SetModelCubesTransform()
	{
		m_cubeModel[0]->position = { 0, 0, 0 };
		m_cubeModel[1]->position = { 0, 0, -1 };
		m_cubeModel[2]->position = { 0, 1, -1 };
		m_cubeModel[3]->position = { 0, 1, -2 };
		m_cubeModel[4]->position = { 1, 1, -2 };
		m_cubeModel[5]->position = { 1, 2, -2 };
		m_cubeModel[6]->position = { 2, 2, -2 };
		m_cubeModel[7]->position = { 3, 2, -2 };
		m_cubeModel[8]->position = { 3, 2, -3 };
		m_cubeModel[9]->position = { 3, 3, -3 };
		m_cubeModel[10]->position = { 3, 1, -2 };
		m_cubeModel[11]->position = { 3, 1, -1 };
		m_cubeModel[12]->position = { 3, 1, 0 };
		m_cubeModel[13]->position = { 3, 0, 0 };
		m_cubeModel[14]->position = { 3, 0, -2 };
		m_cubeModel[15]->position = { 4, 1, 0 };

		setOffset();
	}

	void setOffset()
	{
		for (auto& transform : m_cubeModel)
		{
			transform->position.y += modelYOffset;
			transform->position.x += modelXOffset;
			transform->position.z += modelZOffset;
		}
	}

	void SetWallCubesTransform()
	{
		int wallcount = 0;
		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < 10; y++)
			{
				if (checkWallHolePlacement(x, y))
				{
					m_wallCubes[wallcount]->position = {x, y, 0};
					wallcount++;
				}
			}
		}

	}

	bool checkWallHolePlacement(int x, int y)
	{
		for (auto& transform : m_cubeModel)
		{
			if (transform->position.x == x && transform->position.y == y)
			{
				return false;
			}
		}
		return true;
	}


	

	glm::vec4 color = { 52.0f / 255.0f, 107.0f / 255.0f, 194.0f / 255.0f, 1 };

};