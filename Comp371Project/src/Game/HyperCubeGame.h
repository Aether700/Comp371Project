#pragma once
#include "Axes.h"
#include "Core/Application.h"
#include "Core/Script.h"
#include "Core/Input.h"
#include "Model.h"
#include "Core/Time.h"
#include "../Core/Debug.h"

#include "../Core/SoundManager.h"
#include "../Rendering/CameraController.h"


#include "../GameModel/GameModel.h"
#include "../GameModel/ModelOne.h"
#include "../GameModel/ModelTwo.h"
#include "../GameModel/ModelThree.h"
#include "../GameModel/ModelFour.h"
#include "../GameModel/ModelFive.h"

#include <vector>
#include <random>

//TODO: We should be able to call Application::GetCameraController and then set position and lookAt so that we can have the camera follow the cube object
class HyperCubeGame : public Script
{
public:
	HyperCubeGame()
	{
		m_worldTransform = std::make_shared<Transform>();

		cubeTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/ShinyMetal.jpg");
		wallTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");

		AddModel(new ModelOne(m_worldTransform));
		AddModel(new ModelTwo(m_worldTransform));
		AddModel(new ModelThree(m_worldTransform));
		AddModel(new ModelFour(m_worldTransform));
		AddModel(new ModelFive(m_worldTransform));
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
		//start looping background music
		SoundManager::Play("Resources/Audio/breakout.mp3", true);

		//set starting time
		start_time = Time::GetTime();

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

	}


	void OnRender()
	{

		//update lighting every frame for now
		Renderer3D::AddPointLight(glm::vec3(-10, 10, 0));
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
					std::cout << "Fit!" << std::endl;
					SoundManager::Play("Resources/Audio/109663__grunz__success-low.wav", false);
					m_state = GameState::Fit;
				}
				//otherwise to drop state
				else
				{
					std::cout << "Drop!" << std::endl;
					SoundManager::Play("Resources/Audio/31126__calethos__bump.wav", false);
					score = 0.0f; //failure->score reset
					m_state = GameState::Drop;
				}

			}
			else
			{
				//cube_tr.position.z -= Time::GetDeltaTime() * cube_move_speed_per_second;
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
				selectDisplayModel();
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
				selectDisplayModel();
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
	}

protected:


private:
	Grid* m_grid;

	float score = 0.0;

	float start_time = 0.0;

	float cube_move_speed_per_second = 1.0;

	float rotationInputTimer = 0.0f;
	float rotationInputCooldown = 0.3f; //limit rotations per second

	float wall_thickness = 1.0;

	float animation_frame_time = 0; //used when doing drop or fit animations
	float animation_frame_time_limit = 1.5; //number of seconds to do animation for

	Transform light_pos;

	glm::vec4 cube_color = { 0.1, 0.9, 0.1, 1 };
	glm::vec4 wall_color = { 0.1, 0.2, 0.87, 1 };

	std::shared_ptr<OpenGLCubeMap> cubeTexture;
	std::shared_ptr<OpenGLCubeMap> wallTexture;

	std::shared_ptr<Transform> m_worldTransform;

	std::vector<GameModel*> m_models;
	int m_currModel;

	enum class GameState { Spawn, Rotations, Advance, Fit, Drop, Debug };
	GameState m_state = GameState::Spawn;


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
		return (glm::abs(Random::GetInt()) % 4);
	}

	void AddModel(GameModel* m)
	{
		Application::AddScript(m);
		m_models.push_back(m);
	}

	void selectDisplayModel()
	{
		m_models[m_currModel]->Unselect();
		m_currModel = GetRandom();
		//std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		//m_modelTransform->rotation = { GetRandomRotation(), GetRandomRotation(), 0 };
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
		std::cout << "Cube orientation is " << cube_rot.x << ", " << cube_rot.y << ", " << cube_rot.z << std::endl;
		glm::vec3 cube_rotation_sin = glm::vec3{ glm::sin(cube_rot.x/2.0), glm::sin(cube_rot.y/2.0), cube_rot.z };

		//glm::vec3 cube_rotation_normalized = glm::vec3{ glm::mod(cube_rot.x, glm::pi<float>()), glm::sin(2 * cube_rot.y), cube_rot.z };
		
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

	float getTimeSinceStart()
	{
		return (Time::GetTime() - start_time);
	}
};