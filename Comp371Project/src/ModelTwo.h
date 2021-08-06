#pragma once

#include "Model.h"
#include "Core/Random.h"

class ModelTwo : public Model
{
public:

	ModelTwo(std::shared_ptr<Transform> worldTransform) : Model(worldTransform)
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
		for (auto& transform : m_cubeModel)
		{
			RenderCube(transform->GetTransformMatrix());
		}

		for (int count = 0; count < wallcount; count++)
		{
			RenderWall(m_wallCubes[count]->GetTransformMatrix());
		}
	}

protected:

	virtual void RenderCubeWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
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

	virtual void RenderWallWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		Renderer3D::DrawVoxel(transform, wallTexture, 1.0f, color);
	}

	virtual void DrawOnSelected(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		glm::mat4 WireTransform = transform;
		WireTransform[0][0] = WireTransform[0][0] * 1.0005f;//x
		WireTransform[1][1] = WireTransform[1][1] * 1.0005f;//y
		WireTransform[2][2] = WireTransform[2][2] * 1.0005f;//z
		Renderer3D::DrawWireCube(WireTransform, m_wireMaterial, glm::vec4(1 - color.x, 1 - color.y, 1 - color.z, color.w));// opposite color
	}

private:

	std::shared_ptr<Transform> m_cubeModel[19];
	std::shared_ptr<Transform>  m_wallCubes[100];


	void SetModelCubesTransform()
	{
		//original cube
		m_cubeModel[0]->position = { 0, 0, -1};
		m_cubeModel[1]->position = { 0, 0, 0};
		m_cubeModel[2]->position = { 0, 0, 1};
		m_cubeModel[3]->position = { 1, 0, -1};
		m_cubeModel[4]->position = { 1, 0, 0};
		m_cubeModel[5]->position = { 1, 0, 1 };
		m_cubeModel[6]->position = { -1, 0, -1};
		m_cubeModel[7]->position = { -1, 0, 0};
		m_cubeModel[8]->position = { -1, 0, 1};

		m_cubeModel[9]->position = { 0, 2, -1 };
		m_cubeModel[10]->position = { 0, 2, 0 };
		m_cubeModel[11]->position = { 0, 2, 1 };
		m_cubeModel[12]->position = { 1, 2, -1 };
		m_cubeModel[13]->position = { 1, 2, 0 };
		m_cubeModel[14]->position = { 1, 2, 1 };
		m_cubeModel[15]->position = { -1, 2, 0 };
		m_cubeModel[16]->position = { -1, 2, 1 };

		m_cubeModel[17]->position = { 1,  1,  0 };
		m_cubeModel[18]->position = { 1,  1,  -1 };
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


	int wallcount = 0;
	Material m_wireMaterial = Material(true);
};