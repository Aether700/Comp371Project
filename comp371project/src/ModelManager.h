#ifndef _MODEL_MANAGER_H_
#define _MODEL_MANAGER_H_

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

#include <vector>

class ModelManager : public Script
{
public:

	ModelManager()
	{
		AddModel(new JamilHModel());
		AddModel(new JeanModel());
		AddModel(new MadelaineModel());
		AddModel(new AlecModel());
		AddModel(new JamilModel());
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

		//rotate the current model depending on the arrow key pressed
		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			if (m_currMovementMode == Movement::Rotation)
			{
				m_models[m_currModel]->GetModelTransform()->rotation.y -= m_rotationSpeed * Time::GetDeltaTime();
			}
			else
			{
				m_models[m_currModel]->GetModelTransform()->position.x -= m_translationSpeed * Time::GetDeltaTime();
			}
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			if (m_currMovementMode == Movement::Rotation)
			{
				m_models[m_currModel]->GetModelTransform()->rotation.y += m_rotationSpeed * Time::GetDeltaTime();
			}
			else
			{
				m_models[m_currModel]->GetModelTransform()->position.x += m_translationSpeed * Time::GetDeltaTime();
			}
		}

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			if (m_currMovementMode == Movement::Rotation)
			{
				m_models[m_currModel]->GetModelTransform()->rotation.x += m_rotationSpeed * Time::GetDeltaTime();
			}
			else
			{
				m_models[m_currModel]->GetModelTransform()->position.y += m_translationSpeed * Time::GetDeltaTime();
			}
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			if (m_currMovementMode == Movement::Rotation)
			{
				m_models[m_currModel]->GetModelTransform()->rotation.x -= m_rotationSpeed * Time::GetDeltaTime();
			}
			else
			{
				m_models[m_currModel]->GetModelTransform()->position.y -= m_translationSpeed * Time::GetDeltaTime();
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
		}

		if (Input::IsKeyPressed(GLFW_KEY_J))
		{
			m_models[m_currModel]->GetModelTransform()->scale -= Time::GetDeltaTime() 
				* m_scaleSpeed * glm::vec3{ 1, 1, 1 };
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

		/*pressing the C key toggle between translation and 
		  rotation mode which will change how the arrow keys behave
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

	void ChangeIndex(size_t newIndex)
	{
		if (newIndex < m_models.size())
		{
			m_currModel = newIndex;
		}
	}

	void SetModelsPos()
	{
		glm::vec3 positions[] = {
			{ -45, 10, -45 },
			{ -45, 10,  45 },
			{  45, 10, -45 },
			{  45, 10,  45 },
			{  0, 10,  0 },
		};

		glm::vec3 rotations[] = {
			{ 0,  glm::radians(45.0f), 0 },
			{ 0, -glm::radians(225.0f), 0 },
			{ 0, -glm::radians(45.0f), 0 },
			{ 0,  glm::radians(225.0f), 0 },
			{ 0, 0, 0 },
		};

		for (int i = 0; i < m_models.size() && i < 5; i++)
		{
			m_models[i]->GetModelTransform()->position = positions[i];
			m_models[i]->GetModelTransform()->rotation = rotations[i];
		}
	}

	std::vector<Model*> m_models;
	size_t m_currModel = 0;

	float m_rotationSpeed = 2.0f;
	float m_translationSpeed = 5.0f;
	float m_scaleSpeed = 2.0f;

	float m_shuffleCooldown = 0.2f;
	float m_currShuffle = 0.0f;

	float m_toggleCooldown = 0.2f;
	float m_currToggle = 0.0f;

	Movement m_currMovementMode = Movement::Rotation;
};


#endif
