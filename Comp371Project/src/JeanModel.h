#pragma once

#include "Model.h"
#include <stdlib.h>
#include <time.h>

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
		SetWallCubesTransform('f'); // s sideivew, f frontview to determing in which orientation will the model fit. base on the original cube not the shuffle version
		centerModel(); // center it to the XYZ axis
	}

	void OnRender()
	{

		for (auto& transform : m_cubeModel)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (auto& transform : m_wallCubes)
		{
			transform->scale.z = 0.25f;// to make the wall thinner
			Renderer3D::DrawVoxel(transform->GetTransformMatrix());
		}
	}


private:

	std::shared_ptr<Transform> m_cubeModel[16];
	//wall size = roughly model size x2= 5x2 x 2x5 = 100cubes
	std::shared_ptr<Transform>  m_wallCubes[100];

	// offset of center the model in the wall and easier change if needed
	int modelYOffset = 3;
	int modelXOffset = 3;
	int modelZOffset = 5;


	void SetModelCubesTransform()
	{
		//original cube
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

		centerCubeWithWall();// inorder to center the cube in the wall
	}

	void centerCubeWithWall()
	{
		for (auto& transform : m_cubeModel)
		{
			transform->position.x += modelXOffset;
			transform->position.y += modelYOffset;
			transform->position.z += modelZOffset;
		}
	}

	void SetWallCubesTransform(char orientation)
	{
		int wallcount = 0;
		for (int axis1 = 0; axis1 < 10; axis1++)
		{
			for (int axis2 = 0; axis2 < 10; axis2++)
			{
				if (checkWallHolePlacement(axis1, axis2, orientation))
				{
					m_wallCubes[wallcount]->position = { axis1, axis2, 0 };
					wallcount++;
				}
			}
		}

	}

	bool checkWallHolePlacement(int axis1, int axis2, char orientation)
	{
		if(orientation == 'f')
		for (auto& transform : m_cubeModel)
		{
			if (transform->position.x == axis1 && transform->position.y == axis2)
			{
				return false;
			}
		}
		else if(orientation == 's')
		{
			for (auto& transform : m_cubeModel)
			{
				if (transform->position.y == axis1 && transform->position.z == axis2)
				{
					return false;
				}
			}
		}
		return true;
	}

	virtual void Shuffle() override
	{
		//  size of cube 5x4x4 but for simplisity let make it 5x5x5
		srand(time(NULL));
		bool modelChange = false;
		do
		{
			for (int cubeNum = 0; cubeNum < 16; cubeNum++)
			{
				int newX = rand() % 5 + modelXOffset;
				int newY = rand() % 5 + modelYOffset;
				int newZ = rand() % 5 + modelZOffset;
				int randVal = rand() % 2;
				if (!checkWallHolePlacement(newX, newY, 'f'))
				{
					if (checkWallHolePlacement(newX, newY, newZ))
					{
						m_cubeModel[cubeNum]->position = { newX, newY, newZ };
						modelChange = true;
					}
				}
			}
		} while (!modelChange);// just to ensure that something in the model did change hence a successfull shuffle
	}

	bool checkCubeExist(int x, int y, int z) // to ensure that there isnt more than one cube in a set location
	{
		for (auto& transform : m_cubeModel)
		{
			if (transform->position.y == x && transform->position.z == y && transform->position.z == z)
			{
				return false;
			}
		}
		return true;
	}

	void centerModel()
	{
		for (auto& transform : m_cubeModel)
		{
			transform->position.x -= 3;
			transform->position.y -= 3;
		
		}
		for (auto& transform : m_wallCubes)
		{
			transform->position.x += -3;
			transform->position.y += -3;
		}
	}

	glm::vec4 color = { 52.0f / 255.0f, 107.0f / 255.0f, 194.0f / 255.0f, 1 };

};