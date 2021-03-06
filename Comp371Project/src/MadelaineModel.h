#pragma once
#include "Model.h"

class MadelaineModel : public Model
{
public:

	MadelaineModel(std::shared_ptr<Transform> worldTransform) : Model(worldTransform)
	{
		for(auto& t : m_cubes)
		{
			t = std::make_shared<Transform>();
			t->SetParent(GetModelTransform());
		}

		for (auto& t : m_wall)
		{
			t = std::make_shared<Transform>();
			t->SetParent(GetWallTransform());
		}

		m_lightPos.SetParent(worldTransform);
	}
	
	void OnStart()
	{
		SetCubes();
		SetWall();
	}

	virtual void SetLightPos() override {
		glm::vec3 temp = GetModelTransform()->GetTransformMatrix()[3];
		m_lightPos.position = { temp[0], temp[1] + 30, temp[2] };
	}

	void OnRender()
	{
		//draw object
		for (auto& t : m_cubes)
		{
			RenderCube(t->GetTransformMatrix(), m_color);
		}

		//draw wall
		for (auto& t : m_wall)
		{
			if (!(t->position.x == 0 && t->position.y == 0 && t->position.z == 0))
			{
				RenderWall(t->GetTransformMatrix());
			}
		}
		
		//draw light above model if selected
		if (IsSelected()) {
			Renderer3D::AddPointLight(m_lightPos.position);
		}
		
		
	}

	//implements the bonus mechanic of shuffling the model
	virtual void Shuffle() override
	{
		for(auto& t : m_cubes)
		{
			bool isAssigned = false;

			do
			{
				int x = (Random::GetInt() % 7) - 3;
				int y = (Random::GetInt() % 7) - 3;
				int z = (Random::GetInt() % 7) - 3;

				if (!InModel(x, y, z) && CheckHole(x, y))
				{
					t->position = glm::vec3(x, y, z);
					isAssigned = true;
				}
			} 
			while (!isAssigned);
		}
	}



protected:
	virtual void RenderCubeWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		if (GetRenderingPrimitive() == RenderingPrimitive::Points) {
			Renderer3D::DrawPointCube(transform, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Lines) {
			Renderer3D::DrawWireCube(transform, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Triangles) {
			Renderer3D::DrawVoxel(transform, modelTexture, 1, color);
		}
	}


	virtual void RenderWallWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		Renderer3D::DrawVoxel(transform, wallTexture, 1, color);
	}

	virtual void DrawOnSelected(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override  {
		glm::mat4 outline = glm::scale(transform, { 1.01f, 1.01f, 1.01f });
		Renderer3D::DrawWireCube(outline, m_outlineMat, glm::vec4{ 1.0f, 105.0f/255.0f, 180.0f/255.0f, 1 });
	}




private:

	//sets the position of the individual cubes
	void SetCubes()
	{
		//set transforms of the individual cubes
		m_originalCubes[0].position  = {  0,  0,  0 };
		m_originalCubes[1].position  = {  0,  1,  0 };
		m_originalCubes[2].position  = {  0,  2,  0 };
		m_originalCubes[3].position  = { -1,  0,  0 };
		m_originalCubes[4].position  = {  0,  2,  1 };
		m_originalCubes[5].position  = { -1,  0,  1 };
		m_originalCubes[6].position  = { -2,  0,  1 };
		m_originalCubes[7].position  = { -2,  1,  1 };
		m_originalCubes[8].position  = {  0,  0, -1 };
		m_originalCubes[9].position  = {  1,  0, -1 };
		m_originalCubes[10].position = {  1,  1, -1 };

		for (int i = 0; i < 11; i++)
		{
			m_cubes[i]->position = m_originalCubes[i].position;
		}
	}
	
	void SetWall()
	{
		for (int i = 0; i < 11; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				if (!CheckHole(i - 5, j - 5))
				{
					m_wall[i * 11 + j]->position = { i - 5, j - 5, m_wallZOffset };
				}
			}
		}
	}

	bool CheckHole(int x, int y)
	{
		for (int i = 0; i < 11; i++)
		{
			if (m_originalCubes[i].position.x == x && m_originalCubes[i].position.y == y)
			{
				return true;
			}
		}
		return false;
	}

	bool InModel(int x, int y, int z)
	{
		for (auto& t : m_cubes)
		{
			if (t->position.x == x && t->position.y == y && t->position.z == z)
			{
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<Transform> m_cubes[11];
	Transform m_originalCubes[11];

	Transform m_lightPos;
	

	//11x11 wall = 121 cubes
	std::shared_ptr<Transform> m_wall[121];
	float m_wallZOffset = -8.0f;

	glm::vec4 m_color = { 1.0f, 182.0f / 255.0f, 193.0f / 255.0f, 1.0f };
	Material m_outlineMat = Material(true);
};