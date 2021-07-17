#pragma once
#include "Core/Application.h"
#include "Core/Script.h"
#include "Core/Input.h"
#include "Model.h"
#include "Core/Time.h"

#include "JamilHModel.h"
#include "JeanModel.h"
#include "MadelaineModel.h"

#include <vector>

class ModelManager : public Script
{
public:

	ModelManager()
	{
		AddModel(new JamilHModel());
		AddModel(new JeanModel());
		AddModel(new MadelaineModel());
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
			m_models[m_currModel]->GetModelTransform()->rotation.y -= m_rotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_models[m_currModel]->GetModelTransform()->rotation.y += m_rotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			m_models[m_currModel]->GetModelTransform()->rotation.x += m_rotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			m_models[m_currModel]->GetModelTransform()->rotation.x -= m_rotationSpeed * Time::GetDeltaTime();
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
	}

private:
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

	std::vector<Model*> m_models;
	size_t m_currModel = 0;

	float m_rotationSpeed = 2.0f;
	float m_scaleSpeed = 2.0f;

	float m_shuffleCooldown = 0.2f;
	float m_currShuffle = 0.0f;
};