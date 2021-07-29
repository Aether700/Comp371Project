#ifndef _JEAN_MODEL_H_
#define _JEAN_MODEL_H_

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

	virtual void Shuffle() override
	{
		//  size of cube 5x4x4 but for simplisity let make it 5x5x5
		// reset and shuffle given the original model
		SetModelCubesTransform();
		bool modelChange = true;
		do
		{
			for (int cubeNum = 0; cubeNum < 16; cubeNum++)
			{
				int newX = (glm::abs(Random::GetInt()) % 5) + modelXOffset;
				int newY = (glm::abs(Random::GetInt()) % 5) + modelYOffset;
				int newZ = (glm::abs(Random::GetInt()) % 5) + modelZOffset;
				if (!checkWallHolePlacement(newX, newY, 'f')) // check if the new cube position will still fit in the wall
				{
					if (checkCubeExist(newX, newY, newZ)) // to ensure that cube arent place on top of each other
					{
						m_cubeModel[cubeNum]->position = { newX, newY, newZ };
						modelChange = false;
					}
				}
			}
		} while (modelChange);
	}

private:

	std::shared_ptr<Transform> m_cubeModel[16];
	//wall size = roughly model size x2= 5x2 x 2x5 = 100cubes
	std::shared_ptr<Transform>  m_wallCubes[100];

	// offset of center the model in the wall and easier change if needed
	int modelXOffset = 6;
	int modelYOffset = 6;
	int modelZOffset = 6;


	void SetModelCubesTransform()
	{
		//original cube
		m_cubeModel[0]->position = { -3, -3, 0 };
		m_cubeModel[1]->position = { -3, -3, -1 };
		m_cubeModel[2]->position = { -3, -2, -1 };
		m_cubeModel[3]->position = { -3, -2, -2 };
		m_cubeModel[4]->position = { -2, -2, -2 };
		m_cubeModel[5]->position = { -2, -1, -2 };
		m_cubeModel[6]->position = { -1, -1, -2 };
		m_cubeModel[7]->position = { 0, -1, -2 };
		m_cubeModel[8]->position = { 0, -1, -3 };
		m_cubeModel[9]->position = { 0, 0, -3 };
		m_cubeModel[10]->position = { 0, -2, -2 };
		m_cubeModel[11]->position = { 0, -2, -1 };
		m_cubeModel[12]->position = { 0, -2, 0 };
		m_cubeModel[13]->position = { 0, -2, 0 };
		m_cubeModel[14]->position = { 0, -3, -2 };
		m_cubeModel[15]->position = { 1, -2, 0 };

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
		if (orientation == 'f')
		{
			for (auto& transform : m_cubeModel)
			{
				if (transform->position.x == axis1 && transform->position.y == axis2)
				{
					return false;
				}
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


	bool checkCubeExist(int x, int y, int z) // to ensure that there isnt more than one cube in a set location
	{
		for (auto& transform : m_cubeModel)
		{
			if (transform->position.x == x && transform->position.y == y && transform->position.z == z)
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

#endif
