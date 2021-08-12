#pragma once

#include "GameModel.h"
#include "Core/Random.h"

class ModelFive : public GameModel
{
public:

	ModelFive(std::shared_ptr<Transform> worldTransform) : GameModel(worldTransform)
	{
		for (auto& transform : m_cubeModel)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}

		for (auto& transform : m_wallCubes)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetWallTransform());
		}
	}

	void OnStart()
	{
		SetModelCubesTransform();
		SetWallCubesTransform();
	}

	void OnRender()
	{
		if (m_isSelected)
		{
			for (auto& transform : m_cubeModel)
			{
				RenderCube(transform->GetTransformMatrix());
			}

			for (int count = 0; count < wallcount; count++)
			{
				RenderWall(m_wallCubes[count]->GetTransformMatrix());
			}
		}
	}

	float getScore()
	{
		return 11.0;
	}


protected:


private:

	std::shared_ptr<Transform> m_cubeModel[16];
	std::shared_ptr<Transform>  m_wallCubes[100];


	void SetModelCubesTransform()
	{
		//original cube
		m_cubeModel[0]->position = { -1, -2, 1 };
		m_cubeModel[1]->position = { -1, -2, 0 };
		m_cubeModel[2]->position = { -1, -1, 0 };
		m_cubeModel[3]->position = { -1, -1, -1 };
		m_cubeModel[4]->position = { 0, -1, -1 };
		m_cubeModel[5]->position = { 0, 0, -1 };
		m_cubeModel[6]->position = { 1, 0, -1 };
		m_cubeModel[7]->position = { 2, 1, -2 };
		m_cubeModel[8]->position = { 2, 0, -2 };
		m_cubeModel[9]->position = { 2, 0, -1 };
		m_cubeModel[10]->position = { 2, -1, -1 };
		m_cubeModel[11]->position = { 2, -1, 0 };
		m_cubeModel[12]->position = { 2, -1, 1 };
		m_cubeModel[13]->position = { 2, -1, 1 };
		m_cubeModel[14]->position = { 2, -2, -1 };
		m_cubeModel[15]->position = { 3, -1, 1 };
	}

	void SetWallCubesTransform()
	{
		wallcount = 0;
		for (int axis1 = -4; axis1 < 6; axis1++)
		{
			for (int axis2 = -5; axis2 < 5; axis2++)
			{
				if (checkWallHolePlacement(axis1, axis2))
				{
					m_wallCubes[wallcount]->position = { axis1, axis2, -6 };
					wallcount++;
				}
			}
		}
	}

	bool checkWallHolePlacement(int axis1, int axis2)
	{

		for (auto& transform : m_cubeModel)
		{
			if (transform->position.x == axis1 && transform->position.y == axis2)
			{
				return false;
			}
		}
		return true;
	}
};