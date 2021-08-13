#pragma once
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Rendering/CameraController.h"
#include "Axes.h"
#include "Core/Application.h"
#include "Core/Script.h"
#include "Core/Input.h"
#include "Model.h"
#include "Core/Time.h"
#include "../Core/Debug.h"

#include "../Core/SoundManager.h"
#include "../Rendering/CameraController.h"
#include "../Rendering/Mesh.h"


#include "../GameModel/GameModel.h"
#include "../GameModel/ModelOne.h"
#include "../GameModel/ModelTwo.h"
#include "../GameModel/ModelThree.h"
#include "../GameModel/ModelFour.h"
#include "../GameModel/ModelFive.h"

#include <vector>
#include <random>

class HyperCubeGame : public Script
{
public:
	HyperCubeGame()
	{
	
		backgroundTransform = std::make_shared<Transform>(glm::vec3(0, 25, -100), glm::vec3(0, 0, 0), glm::vec3(200, 100, 1));
		//cubeTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/ShinyMetal.jpg"); NOT BEING USE?
		backgroundTexture = std::make_shared<OpenGLTexture2D>("Resources/Textures/palmTree.jpg");

		AddModel(new ModelOne());
		AddModel(new ModelTwo());
		AddModel(new ModelThree());
		AddModel(new ModelFour());
		AddModel(new ModelFive());

		m_thinkerMesh = Mesh::LoadFromFile("Resources/Models/Lowest_poly_thinker.obj");
	}

	void OnStart()
	{
		//World transform
		Application::AddScript(new Axes(m_worldTransform));

		//Grid
		m_grid = new Grid(m_worldTransform);
		m_grid->ToggleTexture();
		Application::AddScript(m_grid);

		//Cube object initial position
		setCurrentModelPosition(glm::vec3{ 0,1,-1 });

		//Wall position
		setCurrentWallPosition(glm::vec3{ 0,1,-15 });

		for (int i = 0; i < m_models.size(); i++)
		{
			m_models[i]->GetModelTransform()->position = glm::vec3{ 0, 4, -1 };
			m_models[i]->GetWallTransform()->position = glm::vec3{ 0, 5, -15 };
		}

		//light position
		light_tr.position = { -3, 20, 5 };

		//start looping background music
		SoundManager::Play("Resources/Audio/breakout.mp3", true);

		//We do want shadows in this game
		Renderer3D::UseShadows(true);
	}

	void OnUpdate()
	{
		//Cooldown for cube rotations, also only do rotations during the rotations state
		if ((rotationInputTimer >= rotationInputCooldown) && (m_state == GameState::Rotations))
		{
			glm::vec3 model_rotation = getCurrentModelRotation();
			//rotation about x axis (forward/back)
			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				model_rotation.x -= glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
				//std::cout << "Cube orientation is " << model_rotation.x << ", " << model_rotation.y << ", " << model_rotation.z << std::endl;
			}

			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				model_rotation.x += glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
				//std::cout << "Cube orientation is " << model_rotation.x << ", " << model_rotation.y << ", " << model_rotation.z << std::endl;
			}

			//rotation about y axis (left/right)
			if (Input::IsKeyPressed(GLFW_KEY_A))
			{
				model_rotation.y -= glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
				//std::cout << "Cube orientation is " << model_rotation.x << ", " << model_rotation.y << ", " << model_rotation.z << std::endl;
			}

			if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				model_rotation.y += glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
				//std::cout << "Cube orientation is " << model_rotation.x << ", " << model_rotation.y << ", " << model_rotation.z << std::endl;
			}
		}
		else
		{
			rotationInputTimer += Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_SPACE) && (m_state==GameState::Rotations))
		{
			cube_move_speed_per_second = 10.0f;
		}
		else
		{
			cube_move_speed_per_second = 1.0f;
		}

		if (m_currDebugToggleTimer >= m_debugToggleTimer)
		{
			//press shift+C to toggle debug mode
			if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) || Input::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT))
			{
				if (Input::IsKeyPressed(GLFW_KEY_C))
				{
					if (m_state == GameState::Debug)
					{
						m_state = m_lastState;
						Application::GetCameraController()->setLookIsOn(false);
						Application::GetCameraController()->setMovementIsOn(false);
					}
					else
					{
						m_lastState = m_state;
						m_state = GameState::Debug;
					}
					m_currDebugToggleTimer = 0.0f;
				}
			}
			
		}
		else
		{
			m_currDebugToggleTimer += Time::GetDeltaTime();
		}
	}


	void OnRender()
	{

		//background
		Renderer3D::DrawQuad(backgroundTransform->GetTransformMatrix(), backgroundTexture);
		Renderer3D::DrawMesh(m_thinkerTransform1.GetTransformMatrix(), m_thinkerMesh, Renderer3D::GetDefaultWhiteTexture());
		Renderer3D::DrawMesh(m_thinkerTransform2.GetTransformMatrix(), m_thinkerMesh, Renderer3D::GetDefaultWhiteTexture());

		//update lighting every frame for now
		Renderer3D::AddPointLight(light_tr.position);
		Renderer3D::UpdateLights();

		switch (m_state)
		{
		//Cube model spawned, a random non-solution orientation chosen
		case GameState::Spawn:
			selectDisplayModel();
			setCurrentModelPosition(glm::vec3{ 0,5,-1 });
			m_state = GameState::Rotations;

			//Camera is locked to cube movement
			Application::GetCameraController()->setLookIsOn(false);
			Application::GetCameraController()->setMovementIsOn(false);

			break;

			//Player doing rotations, cube moving forward
		case GameState::Rotations:
			if ((getCurrentModelPosition().z + wall_thickness*4 ) <= getCurrentWallPosition().z)
			{
				//reset animation frame time
				animation_frame_time = 0.0;

				//go to fit state if cube fits
				if (IsRotationCorrect())
				{
					//std::cout << "Fit!" << std::endl;
					SoundManager::Play("Resources/Audio/109663__grunz__success-low.wav", false);
					m_state = GameState::Fit;
				}
				//otherwise to drop state
				else
				{
					//std::cout << "Drop!" << std::endl;
					SoundManager::Play("Resources/Audio/31126__calethos__bump.wav", false);
					score = 0.0f; //failure->score reset
					m_state = GameState::Drop;
				}

			}
			else
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.z -= Time::GetDeltaTime() * cube_move_speed_per_second;
				setCurrentModelPosition(model_pos);
			}

			Application::GetCameraController()->SetCamera(getCurrentModelPosition() + glm::vec3{ -1, 3, 9 }, getCurrentModelPosition() + glm::vec3{ 0, 0, 0 }, 15.0f, -40.0f);

			break;

			//Player hit forward button
		case GameState::Advance:
			break;

			//Cube model fits into wall (correct orientation), so bring it through
		case GameState::Fit:
			AddModelScore();
			if (animation_frame_time < animation_frame_time_limit)
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.z -= Time::GetDeltaTime() * cube_move_speed_per_second;
				setCurrentModelPosition(model_pos);
				animation_frame_time += Time::GetDeltaTime();
			}
			else
			{
				m_state = GameState::Spawn;
			}

			break;

			//Cube model doesn't fit into wall (incorrect orientation), so drop it down
		case GameState::Drop:
   			if (animation_frame_time < animation_frame_time_limit / 2)
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.y -= Time::GetDeltaTime() * cube_move_speed_per_second;
				setCurrentModelPosition(model_pos);
				animation_frame_time += Time::GetDeltaTime();
			}
			else
			{
				m_state = GameState::Spawn;
			}

			break;

		case GameState::Debug:
			Application::GetCameraController()->setLookIsOn(true);
			Application::GetCameraController()->setMovementIsOn(true);

			break;

		default:
			break;

		}

		if (m_state != GameState::Debug)
		{
			UpdateGameTime();
		}
	}

	void OnImGuiRender() override{

		ImGui::Begin("Score");
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);

		ImGui::Text("Score : %d",(int) score);
		ImGui::End();

		ImGui::Begin("Time");
		ImGui::Text("Time : %.2f", m_gameTime);
		ImGui::End();

	}

protected:


private:
	Grid* m_grid;

	float score = 0.0;

	float m_gameTime = 0.0f;

	float cube_move_speed_per_second = 1.0;

	float rotationInputTimer = 0.0f;
	float rotationInputCooldown = 0.2f; //limit rotations per second

	float wall_thickness = 1.0;

	float animation_frame_time = 0; //used when doing drop or fit animations
	float animation_frame_time_limit = 1.5; //number of seconds to do animation for

	Transform light_tr;

	glm::vec4 cube_color = { 0.1, 0.9, 0.1, 1 };
	glm::vec4 wall_color = { 0.1, 0.2, 0.87, 1 };

	//std::shared_ptr<OpenGLCubeMap> cubeTexture;

	std::shared_ptr<OpenGLTexture2D> backgroundTexture;

	std::shared_ptr<Transform>  backgroundTransform;

	std::shared_ptr<Transform> m_worldTransform;

	std::vector<GameModel*> m_models;
	int m_currModel;

	enum class GameState { Spawn, Rotations, Advance, Fit, Drop, Debug };
	GameState m_state = GameState::Spawn;
	GameState m_lastState; //used when toggling debugmode

	const float m_debugToggleTimer = 0.2f;
	float m_currDebugToggleTimer = 0.0f;

	std::shared_ptr<Mesh> m_thinkerMesh;
	Transform m_thinkerTransform1 = Transform({ -20, 0, -20 }, { -glm::radians(90.0f), 0, -glm::radians(30.0f) }, { 0.1f, 0.1f, 0.1f });
	Transform m_thinkerTransform2 = Transform({ 20, 0, -20 }, { -glm::radians(90.0f), 0, glm::radians(30.0f) }, { 0.1f, 0.1f, 0.1f });

	//return a random orientation in radians
	//https://stackoverflow.com/a/7560564/9421977
	float GetRandomRotation()
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(0, 3); // define the range
		
		return distr(gen) * glm::radians(90.0f);
	}

	int GetRandom() //get random int from 0 to 4
	{
		return (glm::abs(Random::GetInt()) % 5);
	}

	void AddModel(GameModel* m)
	{
		Application::AddScript(m);
		m_models.push_back(m);
	}

	void selectDisplayModel()
	{
		m_models[m_currModel]->Unselect();
		int lastModel = m_currModel;
		while (lastModel == m_currModel)
		{
			m_currModel = GetRandom();
		}
		setCurrentModelRotation(glm::vec3{ GetRandomRotation(), GetRandomRotation(), 0 });
		m_models[m_currModel]->Select();
	}

	void setCurrentModelPosition(glm::vec3 v)
	{
		std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		m_modelTransform->position = v;
	}

	void setCurrentModelRotation(glm::vec3 v)
	{
		std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		m_modelTransform->rotation = v;
	}

	glm::vec3 getCurrentModelPosition()
	{
		std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		return m_modelTransform->position;
	}

	glm::vec3 getCurrentModelRotation()
	{
		std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		return m_modelTransform->rotation;
	}

	void setCurrentWallPosition(glm::vec3 v)
	{
		std::shared_ptr<Transform> m_WallTransform = m_models[m_currModel]->GetWallTransform();
		m_WallTransform->position = v;
	}

	void setCurrentWallRotation(glm::vec3 v)
	{
		std::shared_ptr<Transform> m_WallTransform = m_models[m_currModel]->GetWallTransform();
		m_WallTransform->rotation = v;
	}

	glm::vec3 getCurrentWallPosition()
	{
		std::shared_ptr<Transform> m_WallTransform = m_models[m_currModel]->GetWallTransform();
		return m_WallTransform->position;
	}

	glm::vec3 getCurrentWallRotation()
	{
		std::shared_ptr<Transform> m_WallTransform = m_models[m_currModel]->GetWallTransform();
		return m_WallTransform->rotation;
	}

	//Since rotation stored as float, need to make approximate comparisons, not exact
	bool IsRotationCorrect()
	{
		glm::vec3 cube_rot = getCurrentModelRotation();
		//std::cout << "Cube orientation is " << cube_rot.x << ", " << cube_rot.y << ", " << cube_rot.z << std::endl;
		glm::vec3 cube_rotation_sin = glm::vec3{ glm::sin(cube_rot.x/2.0), glm::sin(cube_rot.y/2.0), cube_rot.z };

		if ( glm::abs(cube_rotation_sin.x - 0.0) < 0.001 && glm::abs(cube_rotation_sin.y - 0.0) < 0.001)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//Add the score from the current model
	void AddModelScore()
	{
		score += m_models[m_currModel]->getScore();
	}

	void UpdateGameTime()
	{
		m_gameTime += Time::GetDeltaTime();
	}
};