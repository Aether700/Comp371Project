#ifndef _JAMILH_MODEL_H_
#define _JAMILH_MODEL_H_

#include "Model.h"
#include "Core/Random.h"

//Jamil Hanachian's model (there are two Jamil H in the group)
class JamilHModel : public Model
{
public:
	JamilHModel()
	{
		for (auto& transform : m_modelCubes)
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
		for (auto& transform : m_modelCubes)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (unsigned int i = 0; i < m_wallCubeCount; i++)
		{
			Renderer3D::DrawVoxel(m_wallCubes[i]->GetTransformMatrix());
		}
	}

	//shuffles the cube as per the bonus marks requirements of the assignment
	virtual void Shuffle() override
	{
		//shuffle cube
		for (auto& cube : m_modelCubes)
		{
			TryToShuffleCube(cube);
		}

		//recalculate wall
		SetWallCubesTransform();
	}

private:

	void SetModelCubesTransform()
	{
		//set up model by arranging individual cube's transforms
		m_modelCubes[0]->position = { 0,  0,  0 };
		m_modelCubes[1]->position = { 0,  1,  0 };
		m_modelCubes[2]->position = { 0, -1,  0 };
		m_modelCubes[3]->position = { 1,  0,  0 };
		m_modelCubes[4]->position = { -1,  0,  0 };
		m_modelCubes[5]->position = { -1,  1,  0 };
		m_modelCubes[6]->position = { -1,  2,  0 };
		m_modelCubes[7]->position = { -1, -1,  0 };
		m_modelCubes[8]->position = { 1, -1,  0 };
		m_modelCubes[9]->position = { 2, -1,  0 };
		m_modelCubes[10]->position = { 0,  0, -1 };
		m_modelCubes[11]->position = { 2, -1, -1 };
		m_modelCubes[12]->position = { -1,  2, -1 };
		m_modelCubes[13]->position = { -1, -1, -1 };
	}

	void SetWallCubesTransform()
	{
		m_wallCubeCount = 0;
		for (int y = -5; y <= 5; y++)
		{
			for (int x = -5; x <= 5; x++)
			{
				if (!HasCollidingBlock(x, y))
				{
					m_wallCubes[m_wallCubeCount]->position = glm::vec3(x, y, m_wallZOffset);
					m_wallCubeCount++;
				}
			}
		}
	}

	//tries to randomly change the cube's position in the model the position might not be shuffled
	void TryToShuffleCube(std::shared_ptr<Transform>& cube)
	{
		//50% chance we don't move the cube
		if (Random::GetFloat() <= 0.5f)
		{
			return;
		}

		int xPos = (glm::abs(Random::GetInt()) % 5);
		int yPos = (glm::abs(Random::GetInt()) % 5);
		int zPos = (glm::abs(Random::GetInt()) % 5);

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				for (int k = 0; k < 5; k++)
				{
					glm::vec3 tentitativePos = glm::vec3{ (xPos + i - 6), (yPos + j - 6), (zPos + k - 3) };

					if (!HasShuffleCollision(tentitativePos))
					{
						cube->position = tentitativePos;
						break;
					}
				}
			}
		}
	}

	//checks if the provided position will collide with a cube in the model
	bool HasCollidingBlock(float x, float y)
	{
		for (auto& cubeTransform : m_modelCubes)
		{
			if (cubeTransform->position.x == x && cubeTransform->position.y == y)
			{
				return true;
			}
		}
		return false;
	}

	//checks if the provided position collides with other cubes of the model (is used when shuffling)
	bool HasShuffleCollision(const glm::vec3& pos)
	{
		for (auto& cubeTransform : m_modelCubes)
		{
			if (cubeTransform->position == pos)
			{
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<Transform> m_modelCubes[14];

	//wall is 11x11 -> gives us 121 cubes
	std::shared_ptr<Transform> m_wallCubes[121];
	unsigned int m_wallCubeCount;

	float m_wallZOffset = -10.0f;

	glm::vec4 color = { 1, 0, 0, 1 };

};

#endif
