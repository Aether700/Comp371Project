#ifndef _MADELAINE_MODEL_H_
#define _MADELAINE_MODEL_H_

#include "Model.h"

class MadelaineModel : public Model
{
public:

	MadelaineModel()
	{
		for(Transform& t : m_cubes)
		{
			t.SetParent(GetModelTransform());
		}

		for (Transform& t : m_wall)
		{
			t.SetParent(GetModelTransform());
		}
	}
	
	void OnStart()
	{
		SetCubes();
		SetWall();
	}

	void OnRender()
	{
		for (Transform& t : m_cubes)
		{
			RenderCube(t.GetTransformMatrix(), m_color);
		}

		for (Transform& t : m_wall)
		{
			if (!(t.position.x == 0 && t.position.y == 0 && t.position.z == 0))
			{
				Renderer3D::DrawVoxel(t.GetTransformMatrix());
			}
		}
	}

	//implements the bonus mechanic of shuffling the model
	virtual void Shuffle() override
	{
		for(Transform& t : m_cubes)
		{
			bool isAssigned = false;

			do
			{
				int x = (Random::GetInt() % 7) - 3;
				int y = (Random::GetInt() % 7) - 3;
				int z = (Random::GetInt() % 7) - 3;

				if (!InModel(x, y, z) && CheckHole(x, y))
				{
					t.position = glm::vec3(x, y, z);
					isAssigned = true;
				}
			} 
			while (!isAssigned);
		}
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
			m_cubes[i].position = m_originalCubes[i].position;
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
					m_wall[i * 11 + j].position = { i - 5, j - 5, m_wallZOffset };
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
		for (Transform& t : m_cubes)
		{
			if (t.position.x == x && t.position.y == y && t.position.z == z)
			{
				return true;
			}
		}
		return false;
	}

	Transform m_cubes[11];
	Transform m_originalCubes[11];

	//11x11 wall = 121 cubes
	Transform m_wall[121];
	float m_wallZOffset = -8.0f;

	glm::vec4 m_color = { 1.0f, 182.0f / 255.0f, 193.0f / 255.0f, 1.0f };
};

#endif
