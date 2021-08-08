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
		cube_tr.position = glm::vec3{ 0, 1, -1 };

		//Wall position
		wall_tr.position = glm::vec3{ 0, 2, -15 };
		wall_tr.scale = glm::vec3{ 4, 4, 1 };
		
		for (int i = 0; i < m_models.size(); i++)
		{
			m_models[i]->GetModelTransform()->position = glm::vec3{ 0, 4, -1 };
			m_models[i]->GetWallTransform()->position = glm::vec3{ 0, 5, -15 };
		}
		//start looping background music
		SoundManager::Play("Resources/Audio/breakout.mp3", true);
	}

	void OnUpdate()
	{
		//Cooldown for cube rotations, also only do rotations during the rotations state
		if ( (rotationInputTimer >= rotationInputCooldown) && (m_state == GameState::Rotations) )
		{
			//rotation about x axis (forward/back)
			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				cube_tr.rotation.x += glm::radians(90.0f);
				rotationInputTimer = 0.0f;
				std::cout << "Cube orientation is " << cube_tr.rotation.x << ", " << cube_tr.rotation.y << ", " << cube_tr.rotation.z << std::endl;
			}

			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				cube_tr.rotation.x -= glm::radians(90.0f);
				rotationInputTimer = 0.0f;
				std::cout << "Cube orientation is " << cube_tr.rotation.x << ", " << cube_tr.rotation.y << ", " << cube_tr.rotation.z << std::endl;
			}

			//rotation about y axis (left/right)
			if (Input::IsKeyPressed(GLFW_KEY_A))
			{
				cube_tr.rotation.y -= glm::radians(90.0f);
				rotationInputTimer = 0.0f;
				std::cout << "Cube orientation is " << cube_tr.rotation.x << ", " << cube_tr.rotation.y << ", " << cube_tr.rotation.z << std::endl;
			}

			if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				cube_tr.rotation.y += glm::radians(90.0f);
				rotationInputTimer = 0.0f;
				std::cout << "Cube orientation is " << cube_tr.rotation.x << ", " << cube_tr.rotation.y << ", " << cube_tr.rotation.z << std::endl;
			}

		}
		else
		{
			rotationInputTimer += Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		{
			cube_move_speed_per_second = 10.0f;
		}
		else
		{
			cube_move_speed_per_second = 1.0f;
		}

	}

	//TODO: have Model objects for wall and cube, so HyperCubeGameWall and HyperCubeGameCube
	//then we would simply set the position of the cube object to move it
	//and then wall object would stay in place
	void OnRender()
	{

		//update lighting every frame for now
		Renderer3D::AddPointLight(glm::vec3(-10, 10, 0));
		Renderer3D::UseShadows(true);
		Renderer3D::UpdateLights();

		switch(m_state)
		{
			//Cube model spawned, a random non-solution orientation chosen
			case GameState::Spawn:
				selectDisplayModel();
				cube_tr.position = { 0,1,-1 };
				cube_tr.rotation = { GetRandomRotation(), GetRandomRotation(), 0 };
				std::cout << "Cube orientation is " << cube_tr.rotation.x << ", " << cube_tr.rotation.y << ", " << cube_tr.rotation.z << std::endl;
				cube_tr.scale = { 1,1,1 };
				Renderer3D::DrawVoxel(cube_tr.GetTransformMatrix(), cubeTexture, 1, cube_color);
				Renderer3D::DrawVoxel(wall_tr.GetTransformMatrix(), wall_color);
				m_state = GameState::Rotations;
				Application::GetCameraController()->setLookIsOn(false);
				Application::GetCameraController()->setMovementIsOn(false);

				break;

			//Player doing rotations, cube moving forward
			case GameState::Rotations:
				Renderer3D::DrawVoxel(cube_tr.GetTransformMatrix(), cubeTexture, 1, cube_color);
 				Renderer3D::DrawVoxel(wall_tr.GetTransformMatrix(), wall_color);

				//TODO: probably want some function that evaluates whether the cube has reached the wall
				if ( (cube_tr.position.z - wall_thickness) <= wall_tr.position.z)
				{
					//go to fit state if cube fits
					//if (glm::vec3{ glm::sin(cube_tr.rotation.x), glm::sin(cube_tr.rotation.y), cube_tr.rotation.z } == glm::vec3{ 0,0,0 })
					if(IsRotationCorrect())
					{
						std::cout << "Fit!" << std::endl;
						m_state = GameState::Fit;
					}
					//otherwise to drop state
					else
					{
						std::cout << "Drop!" << std::endl;
						m_state = GameState::Drop;
					}

				}
				else
				{
					cube_tr.position.z -= Time::GetDeltaTime() * cube_move_speed_per_second;
				}

				Application::GetCameraController()->SetCamera(cube_tr.position + glm::vec3{ 0, 1, 4 }, cube_tr.position + glm::vec3{0, 1, 0}, 0.0f, 20.0f);

				break;

			//Player hit forward button
			case GameState::Advance:
				break;

			//Cube model fits into wall (correct orientation), so bring it through
			case GameState::Fit:
				//TODO: slowly move through the slot
				Renderer3D::DrawVoxel(cube_tr.GetTransformMatrix(), cubeTexture, 1, cube_color);
				Renderer3D::DrawVoxel(wall_tr.GetTransformMatrix(), wall_color);
				
				Application::GetCameraController()->setLookIsOn(true);
				Application::GetCameraController()->setMovementIsOn(true);
				
				//m_state = GameState::Spawn;
				break;

			//Cube model doesn't fit into wall (incorrect orientation), so drop it down
			case GameState::Drop:
				//TODO: drop down in a gravity-like fashion
				Renderer3D::DrawVoxel(cube_tr.GetTransformMatrix(), cubeTexture, 1, cube_color);
				Renderer3D::DrawVoxel(wall_tr.GetTransformMatrix(), wall_color);
				
				Application::GetCameraController()->setLookIsOn(true);
				Application::GetCameraController()->setMovementIsOn(true);
				
				//m_state = GameState::Spawn;
				break;

			default:
				break;

		}
	}

protected:


private:
	Grid* m_grid;

	//rotation in z is not allowed, and would not be compatible with the typical 4-way control systems (eg WASD, arrows, controller stick)
	int y_rot_count = 0;
	int x_rot_count = 0;
	float y_rot = 0.0f;
	float x_rot = 0.0f;

	float cube_move_speed_per_second = 1.0;

	float rotationInputTimer = 0.0f;
	float rotationInputCooldown = 0.3f; //limit rotations per second

	float wall_thickness = 1.0;

	Transform cube_tr;
	Transform wall_tr;
	Transform light_pos;

	glm::vec4 cube_color = { 0.1, 0.9, 0.1, 1 };
	glm::vec4 wall_color = { 0.1, 0.2, 0.87, 1 };

	std::shared_ptr<OpenGLCubeMap> cubeTexture;
	std::shared_ptr<OpenGLCubeMap> wallTexture;

	std::shared_ptr<Transform> m_worldTransform;

	std::vector<GameModel*> m_models;
	int m_currModel;

	enum class GameState { Spawn, Rotations, Advance, Fit, Drop };
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
		std::shared_ptr<Transform> m_modelTransform = m_models[m_currModel]->GetModelTransform();
		m_modelTransform->rotation = { GetRandomRotation(), GetRandomRotation(), 0 };
		m_models[m_currModel]->Select();
	}

	//Since rotation stored as float, need to make approximate comparisons, not exact
	bool IsRotationCorrect()
	{
		glm::vec3 cube_rotation_sin = glm::vec3{ glm::sin(cube_tr.rotation.x), glm::sin(cube_tr.rotation.y), cube_tr.rotation.z };
		if ( glm::abs(cube_rotation_sin.x - 0.0) < 0.001 && glm::abs(cube_rotation_sin.y - 0.0) < 0.001)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

};

//Nice to have:
//Skybox, fairly muted color
//


//Too nice to have:
//maybe fog in the distance?