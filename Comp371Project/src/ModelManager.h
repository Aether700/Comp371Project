#pragma once
#include "Core/Application.h"
#include "Core/Script.h"
#include "Core/Input.h"
#include "Model.h"
#include "Core/Time.h"

#include "JamilHModel.h"
#include "JamilModel.h"
#include "JeanModel.h"
#include "MadelaineModel.h"
#include "AlecModel.h"

#include "Grid.h"

#include <vector>

class ModelManager : public Script
{
public:

	ModelManager()
	{
		m_worldTransform = std::make_shared<Transform>();

		AddModel(new JamilHModel(m_worldTransform));
		AddModel(new JeanModel(m_worldTransform));
		AddModel(new MadelaineModel(m_worldTransform));
		AddModel(new AlecModel(m_worldTransform));
		AddModel(new JamilModel(m_worldTransform));

		AddGrid(new Grid(m_worldTransform));
	}

	void OnStart()
	{
		SetModelsPos();
	}

	void OnUpdate()
	{
		//numbers 1 throught 5 changes which model we are affecting
		if (Input::IsKeyPressed(GLFW_KEY_1))
		{
			ChangeIndex(0);
		}

		if (Input::IsKeyPressed(GLFW_KEY_2))
		{
			ChangeIndex(1);
		}

		if (Input::IsKeyPressed(GLFW_KEY_3))
		{
			ChangeIndex(2);
		}

		if (Input::IsKeyPressed(GLFW_KEY_4))
		{
			ChangeIndex(3);
		}

		if (Input::IsKeyPressed(GLFW_KEY_5))
		{
			ChangeIndex(4);
		}

		if (m_currModelToggle >= m_toggleModelCooldown && Input::IsKeyPressed(GLFW_KEY_M))
		{
			isOn = !isOn;

			m_currModelToggle = 0.0f;
		}
		else if (m_currModelToggle < m_toggleModelCooldown)
		{
			m_currModelToggle += Time::GetDeltaTime();
		}

		if (isOn)
		{
			//rotate the current model depending on the arrow key pressed
			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				if (m_currMovementMode == Movement::Rotation)
				{
					m_models[m_currModel]->GetModelTransform()->rotation.y -= m_rotationSpeed * Time::GetDeltaTime();
				}
				else
				{
					m_models[m_currModel]->GetModelTransform()->position.y -= m_translationSpeed * Time::GetDeltaTime();
				}
			}

			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				if (m_currMovementMode == Movement::Rotation)
				{
					m_models[m_currModel]->GetModelTransform()->rotation.y += m_rotationSpeed * Time::GetDeltaTime();
				}
				else
				{
					m_models[m_currModel]->GetModelTransform()->position.y += m_translationSpeed * Time::GetDeltaTime();
				}
			}

			if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				if (m_currMovementMode == Movement::Rotation)
				{
					m_models[m_currModel]->GetModelTransform()->rotation.x += m_rotationSpeed * Time::GetDeltaTime();
				}
				else
				{
					m_models[m_currModel]->GetModelTransform()->position.x += m_translationSpeed * Time::GetDeltaTime();
				}
			}

			if (Input::IsKeyPressed(GLFW_KEY_A))
			{
				if (m_currMovementMode == Movement::Rotation)
				{
					m_models[m_currModel]->GetModelTransform()->rotation.x -= m_rotationSpeed * Time::GetDeltaTime();
				}
				else
				{
					m_models[m_currModel]->GetModelTransform()->position.x -= m_translationSpeed * Time::GetDeltaTime();
				}
			}

			if (Input::IsKeyPressed(GLFW_KEY_Q))
			{
				m_models[m_currModel]->GetModelTransform()->rotation.z -= m_rotationSpeed * Time::GetDeltaTime();
			}

			if (Input::IsKeyPressed(GLFW_KEY_E))
			{
				m_models[m_currModel]->GetModelTransform()->rotation.z += m_rotationSpeed * Time::GetDeltaTime();
			}

			if (Input::IsKeyPressed(GLFW_KEY_O)) //one small step foward
			{
				m_models[m_currModel]->GetModelTransform()->position.z -= m_translationSpeed * Time::GetDeltaTime();
			}

			if (m_currMovementToggle >= m_toggleMovementCooldown && Input::IsKeyPressed(GLFW_KEY_I)) // toggle continuos step fowards
			{
				isContinuos = !isContinuos;
				m_currMovementToggle = 0.0f;
			}
			else if (m_currMovementToggle < m_toggleMovementCooldown)
			{
				m_currMovementToggle += Time::GetDeltaTime();
			}

			if (isContinuos == true)
			{
				if (m_models[m_currModel]->GetModelTransform()->position.z > 0)
				{
					m_models[m_currModel]->GetModelTransform()->position.z += m_translationSpeed * Time::GetDeltaTime();
				}
				else
				{
					m_models[m_currModel]->GetModelTransform()->position.z -= m_translationSpeed * Time::GetDeltaTime();
				}
			}
			/*pressing the C key toggle between translation and
			  rotation mode which will change how the awsd keys behave
			*/
			if (m_currToggle >= m_toggleCooldown && Input::IsKeyPressed(GLFW_KEY_C))
			{
				if (m_currMovementMode == Movement::Rotation)
				{
					m_currMovementMode = Movement::Translation;
				}
				else
				{
					m_currMovementMode = Movement::Rotation;
				}

				m_currToggle = 0.0f;
			}
			else if (m_currToggle < m_toggleCooldown)
			{
				m_currToggle += Time::GetDeltaTime();
			}
		}

		//pressing P, T or L changes the rendering primitive used to render the model
		if (Input::IsKeyPressed(GLFW_KEY_P))
		{
			m_models[m_currModel]->SetRenderingPrimitive(RenderingPrimitive::Points);
		}

		if (Input::IsKeyPressed(GLFW_KEY_T))
		{
			m_models[m_currModel]->SetRenderingPrimitive(RenderingPrimitive::Triangles);
		}

		if (Input::IsKeyPressed(GLFW_KEY_L))
		{
			m_models[m_currModel]->SetRenderingPrimitive(RenderingPrimitive::Lines);
		}

		//pressing U and J will scale up and down the model as specified in the assignment
		if (Input::IsKeyPressed(GLFW_KEY_U))
		{
			m_models[m_currModel]->GetModelTransform()->scale += Time::GetDeltaTime()
				* m_scaleSpeed * glm::vec3{ 1, 1, 1 };

			//m_models[m_currModel]->GetWallTransform()->scale += Time::GetDeltaTime()
			//	* m_scaleSpeed * glm::vec3{ 1, 1, 1 };
		}

		if (Input::IsKeyPressed(GLFW_KEY_J))
		{
			m_models[m_currModel]->GetModelTransform()->scale -= Time::GetDeltaTime()
				* m_scaleSpeed * glm::vec3{ 1, 1, 1 };
		}

		//reset the model to original position
		if (Input::IsKeyPressed(GLFW_KEY_G))
		{
			//set model back to original size/orientation/position
			m_models[m_currModel]->GetModelTransform()->position = positions[m_currModel];
			m_models[m_currModel]->GetModelTransform()->rotation = rotations[m_currModel];
			m_models[m_currModel]->GetModelTransform()->scale = { 1.0,1.0,1.0 };

			//set wall back to original size/orientation/position
			m_models[m_currModel]->GetWallTransform()->position = positions[m_currModel];
			m_models[m_currModel]->GetWallTransform()->rotation = rotations[m_currModel];
			m_models[m_currModel]->GetWallTransform()->scale = { 1.0,1.0,1.0 };
		}

		//reset all models to original position
		if (Input::IsKeyPressed(GLFW_KEY_H))
		{
			for (int i = 0; i < m_models.size() && i < 5; i++)
			{
				//set model back to original size/orientation/position
				m_models[i]->GetModelTransform()->position = positions[i];
				m_models[i]->GetModelTransform()->rotation = rotations[i];
				m_models[i]->GetModelTransform()->scale = { 1.0,1.0,1.0 };

				//set wall back to original size/orientation/position
				m_models[i]->GetWallTransform()->position = positions[i];
				m_models[i]->GetWallTransform()->rotation = rotations[i];
				m_models[i]->GetWallTransform()->scale = { 1.0,1.0,1.0 };
			}
			m_worldTransform->position = glm::vec3(0, 0, 0);
			m_worldTransform->rotation = glm::vec3(0, 0, 0);
			m_worldTransform->scale = glm::vec3(1, 1, 1);
		}

		if (m_currTextureTime >= m_textureCooldown && Input::IsKeyPressed(GLFW_KEY_X))
		{
			for (int i = 0; i < m_models.size() && i < 5; i++)
			{
				m_models[i]->ToggleTexture();
			}
			m_grid[0]->ToggleTexture();
			m_currTextureTime = 0.0f;
		}
		else if (m_currTextureTime < m_textureCooldown)
		{
			m_currTextureTime += Time::GetDeltaTime();
		}

		/*pressing the Z key will attempt to shuffle the current model.
		  if the current model does not support the shuffle operation a
		  message indicating so will be displayed in the console
		*/
		if (m_currShuffle >= m_shuffleCooldown && Input::IsKeyPressed(GLFW_KEY_Z))
		{
			m_models[m_currModel]->Shuffle();
			m_currShuffle = 0.0f;
		}
		else if (m_currShuffle < m_shuffleCooldown)
		{
			m_currShuffle += Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
		
			m_worldTransform->position.z += m_translationSpeed * Time::GetDeltaTime();
	
		}
		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{

			m_worldTransform->position.z -= m_translationSpeed * Time::GetDeltaTime();

		}
		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{

			m_worldTransform->position.x -= m_translationSpeed * Time::GetDeltaTime();

		}
		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
	
			m_worldTransform->position.x += m_translationSpeed * Time::GetDeltaTime();
		
		}
		
		if (Input::IsKeyPressed(GLFW_KEY_9))
		{

			m_worldTransform->rotation.y -= m_translationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_0))
		{
			m_worldTransform->rotation.y += m_translationSpeed * Time::GetDeltaTime();
		}
		
	}

private:

	enum class Movement
	{
		Rotation,
		Translation
	};

	void AddModel(Model* m)
	{
		Application::AddScript(m);
		m_models.push_back(m);
	}

	void AddGrid(Grid* g)
	{
		Application::AddScript(g);
		m_grid.push_back(g);
	}

	void ChangeIndex(size_t newIndex)
	{
		if (newIndex < m_models.size())
		{
			m_currModel = newIndex;
		}
	}

	void SetModelsPos()
	{

		for (int i = 0; i < m_models.size() && i < 5; i++)
		{
			m_models[i]->GetModelTransform()->position = positions[i];
			m_models[i]->GetModelTransform()->rotation = rotations[i];
			m_models[i]->GetWallTransform()->position = positions[i];
			m_models[i]->GetWallTransform()->rotation = rotations[i];
		}


	}

	std::vector<Model*> m_models;
	std::vector<Grid*> m_grid;
	size_t m_currModel = 0;

	float m_rotationSpeed = 2.0f;
	float m_translationSpeed = 5.0f;
	float m_scaleSpeed = 2.0f;

	float m_shuffleCooldown = 0.2f;
	float m_currShuffle = 0.0f;

	float m_toggleCooldown = 0.2f;
	float m_currToggle = 0.0f;

	float m_toggleMovementCooldown = 0.2f;
	float m_currMovementToggle = 0.0f;

	float m_toggleModelCooldown = 0.2f;
	float m_currModelToggle = 0.0f;

	float m_textureCooldown = 0.2f;
	float m_currTextureTime = 0.0f;

	float m_toggleWorlMovementCooldown = 0.2f;
	float m_currWorlMovementToggle = 0.0f;

	Movement m_currMovementMode = Movement::Rotation;
	bool isOn = false;
	bool isContinuos = false;
	bool isWorldMovement = false;

	std::shared_ptr<Transform> m_worldTransform;

	glm::vec3 positions[5] = {
	{ -40, 10, -40 },
	{ -40, 10,  40 },
	{  40, 10, -40 },
	{  40, 10,  40 },
	{  0, 10,  0 },
	};

	glm::vec3 rotations[5] = {
		{ 0,  glm::radians(0.0f), 0 },
		{ 0, -glm::radians(180.0f), 0 },
		{ 0,  -glm::radians(0.0f), 0 },
		{ 0,  glm::radians(180.0f), 0 },
		{ 0, 0, 0 },
	};
};