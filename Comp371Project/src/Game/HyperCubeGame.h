#pragma once
#include "Axes.h"
#include "Core/Application.h"
#include "Core/Script.h"
#include "Core/Input.h"
#include "Model.h"
#include "Core/Time.h"

#include <vector>

//TODO: We should be able to call Application::GetCameraController and then set position and lookAt so that we can have the camera follow the cube object
class HyperCubeGame : public Script
{
public:
	HyperCubeGame()
	{
		cubeTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/ShinyMetal.jpg");
		wallTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");
	}

	void OnStart()
	{
		//World transform
		std::shared_ptr<Transform> m_worldTransform = std::make_shared<Transform>();
		Application::AddScript(new Axes(m_worldTransform));

		//Grid
		m_grid = new Grid(m_worldTransform);
		m_grid->ToggleTexture();
		Application::AddScript(m_grid);

		//Cube object initial position
		cube_tr.position = glm::vec3{ 0, 1, -1 };

		//Wall position
		wall_tr.position = glm::vec3{ 0, 2, -10 };
		wall_tr.scale = glm::vec3{ 4, 4, 1 };
		

	}

	void OnUpdate()
	{

		//Cooldown for cube rotations, also only do rotations during the rotations state
		if ( (rotationInputTimer >= rotationInputCooldown) && (m_state == GameState::Rotations) )
		{
			//rotation about x axis (forward/back)
			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				cube_tr.rotation.z += glm::radians(90.0f);
				rotationInputTimer = 0.0f;
			}

			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				cube_tr.rotation.z -= glm::radians(90.0f);
				rotationInputTimer = 0.0f;
			}

			//rotation about y axis (left/right)
			if (Input::IsKeyPressed(GLFW_KEY_A))
			{
				cube_tr.rotation.y += glm::radians(90.0f);
				rotationInputTimer = 0.0f;
			}

			if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				cube_tr.rotation.y -= glm::radians(90.0f);
				rotationInputTimer = 0.0f;
			}

		}
		else
		{
			rotationInputTimer += Time::GetDeltaTime();
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
				cube_tr.position = { 0,1,-1 };
				cube_tr.rotation = { 0, 0, 0 };
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
				if (cube_tr.position.z <= wall_tr.position.z)
				{
					//go to fit state where the cube fitting or not will be evaluated
					m_state = GameState::Fit;
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
				Renderer3D::DrawVoxel(cube_tr.GetTransformMatrix(), cubeTexture, 1, cube_color);
				Renderer3D::DrawVoxel(wall_tr.GetTransformMatrix(), wall_color);
				Application::GetCameraController()->setLookIsOn(true);
				Application::GetCameraController()->setMovementIsOn(true);
				break;

			//Cube model doesn't fit into wall (incorrect orientation), so drop it down
			case GameState::Drop:
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

	Transform cube_tr;
	Transform wall_tr;
	Transform light_pos;

	glm::vec4 cube_color = { 0.1, 0.9, 0.1, 1 };
	glm::vec4 wall_color = { 0.1, 0.2, 0.87, 1 };

	std::shared_ptr<OpenGLCubeMap> cubeTexture;
	std::shared_ptr<OpenGLCubeMap> wallTexture;

	enum class GameState { Spawn, Rotations, Advance, Fit, Drop };
	GameState m_state = GameState::Spawn;

};

//Nice to have:
//Skybox, fairly muted color
//


//Too nice to have:
//maybe fog in the distance?