#pragma once

#include "Model.h"
#include "Core/Random.h"

class JeanModel : public Model
{
public:

	JeanModel(std::shared_ptr<Transform> worldTransform) : Model(worldTransform)
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
		SetWallCubesTransform('f'); // s sideivew, f frontview to determing in which orientation will the model fit. base on the original cube not the shuffle version
	}

	void OnRender()
	{
		for (auto& transform : m_cubeModel)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (int count = 0; count < wallcount; count++)
		{
			//transform->scale.z = 0.25f;// to make the wall thinner
			RenderWall(m_wallCubes[count]->GetTransformMatrix());
		}
		Renderer3D::AddDirectionalLight(ModelPosition() + glm::vec3(0,30,0) ,glm::vec3(0,-1,0));
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

	void RenderCubeWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		if (GetRenderingPrimitive() == RenderingPrimitive::Triangles)
		{	
			 Renderer3D::DrawVoxel(transform, modelTexture, 1.0f, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Lines)
		{
			Renderer3D::DrawWireCube(transform, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Points)
		{
			Renderer3D::DrawPointCube(transform, color);
		}
	}

	void RenderWallWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		Renderer3D::DrawVoxel(transform, wallTexture, 1.0f, color);

	}

	glm::vec3 ModelPosition()
	{
		glm::mat4 modelTransform = GetModelTransform()->GetTransformMatrix();
		return glm::vec3(modelTransform[3][0], modelTransform[3][1], modelTransform[3][2]);
	}


private:

	std::shared_ptr<Transform> m_cubeModel[16];
	//wall size = roughly model size x2= 5x2 x 2x5 = 100cubes
	std::shared_ptr<Transform>  m_wallCubes[100];

	// offset of center the model in the wall and easier change if needed
	int modelXOffset = -2;
	int modelYOffset = -2;
	int modelZOffset = -2;


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

	void SetWallCubesTransform(char orientation)
	{
		wallcount = 0;
		for (int axis1 = -4; axis1 < 6; axis1++)
		{
			for (int axis2 = -5; axis2 < 5; axis2++)
			{
				if (checkWallHolePlacement(axis1, axis2, orientation))
				{
					m_wallCubes[wallcount]->position = { axis1, axis2, -6 };
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

	int wallcount = 0;
	glm::vec4 color = { 52.0f / 255.0f, 107.0f / 255.0f, 194.0f / 255.0f, 1.0f };

};