#pragma once
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Rendering/CameraController.h"
#include "../Axes.h"
#include "../Core/Application.h"
#include "../Core/Script.h"
#include "../Core/Input.h"
#include "../Model.h"
#include "../Core/Time.h"
#include "../Core/Debug.h"
#include "../Core/Random.h"
#include "../Grid.h"

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

#define UPDATE_STREAK_FACTOR 3

class HyperCubeGame : public Script
{
public:
	HyperCubeGame()
	{
	
		backgroundTransform = std::make_shared<Transform>(glm::vec3(0, 25, -100), glm::vec3(0, 0, 0), glm::vec3(200, 100, 1));
		backgroundTexture = std::make_shared<OpenGLTexture2D>("Resources/Textures/palmTree.jpg");
		sandTexture = std::make_shared<OpenGLTexture2D>("Resources/Textures/sand.jpg");
		nightTexture = std::make_shared<OpenGLTexture2D>("Resources/Textures/night.jpg");

		
		AddModel(new ModelOne());
		AddModel(new ModelTwo());
		AddModel(new ModelThree());
		AddModel(new ModelFour());
		AddModel(new ModelFive());

		m_thinkerMesh = Mesh::LoadFromFile("Resources/Models/Lowest_poly_thinker.obj");
		m_ColMesh = Mesh::LoadFromFile("Resources/Models/Lowpoly_Ionic_Column.obj");

		//increase font size
		ImGui::GetIO().FontGlobalScale = 1.5f;
		
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
		
		//We do want shadows in this game
		Renderer3D::UseShadows(true);

		//start looping background music
		SoundManager::Play("Resources/Audio/breakout.mp3", true);
	}

	void OnStart()
	{
		LoadMenu();
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
			}

			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				model_rotation.x += glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
			}

			//rotation about y axis (left/right)
			if (Input::IsKeyPressed(GLFW_KEY_A))
			{
				model_rotation.y -= glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
			}

			if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				model_rotation.y += glm::radians(90.0f);
				setCurrentModelRotation(model_rotation);
				rotationInputTimer = 0.0f;
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
		if (m_state != GameState::MainMenu)
		{
			//background
			Renderer3D::DrawQuad(backgroundTransform->GetTransformMatrix(), backgroundTexture);

			//objects
			Renderer3D::DrawMesh(m_thinkerTransform1.GetTransformMatrix(), m_thinkerMesh, nightTexture);
			Renderer3D::DrawMesh(m_thinkerTransform2.GetTransformMatrix(), m_thinkerMesh, nightTexture);
			Renderer3D::DrawMesh(m_colTransform1.GetTransformMatrix(), m_ColMesh, nightTexture);
			Renderer3D::DrawMesh(m_colTransform2.GetTransformMatrix(), m_ColMesh, nightTexture);
			Renderer3D::DrawMesh(m_colTransform3.GetTransformMatrix(), m_ColMesh, nightTexture);
			Renderer3D::DrawMesh(m_colTransform4.GetTransformMatrix(), m_ColMesh, nightTexture);


			Renderer3D::AddDirectionalLight({ 0,0,-50 }, { 0,30,-15 }, { 1,14.0f / 255.0f,246.0f / 255.0f,1 },
				m_state == GameState::Debug);
			Renderer3D::AddPointLight(light_tr.position, { 0,0,1,1 }, m_state == GameState::Debug);
			Renderer3D::AddDirectionalLight(glm::vec3{ 0,50,50 }, glm::vec3{ 50,0,50 },
				glm::vec4{ 1,14.0f / 255.0f,246.0f / 255.0f,1 }, m_state == GameState::Debug);
		}

		//do not update light in debug mode
		if (m_state != GameState::Debug)
		{
			Renderer3D::UpdateLights();
		}

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
					SoundManager::Play("Resources/Audio/109663__grunz__success-low.wav", false);
					m_state = GameState::Fit;
					AddModelScore();
				}
				//otherwise to drop state
				else
				{
					SoundManager::Play("Resources/Audio/31126__calethos__bump.wav", false);
					m_state = GameState::Drop;
				}

			}
			else
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.z -= Time::GetDeltaTime() * cube_move_speed_per_second * m_speedUpFactor;
				setCurrentModelPosition(model_pos);
			}

			Application::GetCameraController()->SetCamera(getCurrentModelPosition() + m_camOffset, getCurrentModelPosition(), 15.0f, -40.0f);

			break;

			//Player hit forward button
		case GameState::Advance:
			break;

			//Cube model fits into wall (correct orientation), so bring it through
		case GameState::Fit:
			
			if (animation_frame_time < fit_animation_frame_time_limit)
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.z -= Time::GetDeltaTime() * m_fitSpeed;
				setCurrentModelPosition(model_pos);
				animation_frame_time += Time::GetDeltaTime();
				if (!m_uiUpdated)
				{
					m_streakNum++;
					UpdateSpeed();
					UpdateMultiplicator();
					m_uiUpdated = true;
				}
			}
			else
			{
				m_state = GameState::Spawn;
				m_uiUpdated = false; //reset ui bool
			}

			break;

			//Cube model doesn't fit into wall (incorrect orientation), so drop it down
		case GameState::Drop:
			if (animation_frame_time < drop_animation_frame_time_limit)
			{
				glm::vec3 model_pos = getCurrentModelPosition();
				model_pos.y -= Time::GetDeltaTime() * m_dropSpeed;
				setCurrentModelPosition(model_pos);
				animation_frame_time += Time::GetDeltaTime();

				if (!m_uiUpdated)
				{
					m_streakNum = 0;
					ResetSpeedAndMultiplicator();
					m_uiUpdated = true;
				}
			}
			else
			{
				m_state = GameState::Spawn;
				m_uiUpdated = false; //reset ui bool
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

	void OnImGuiRender() override
	{
		if (m_state == GameState::MainMenu)
		{
			RenderMainMenu();
		}
		else 
		{
			RenderGameUI();
		}
	}

private:
	Grid* m_grid;

	float score = 0.0;

	float m_gameTime = 0.0f;

	float cube_move_speed_per_second = 1.0;

	float rotationInputTimer = 0.0f;
	float rotationInputCooldown = 0.2f; //limit rotations per second

	float wall_thickness = 1.0;

	float animation_frame_time = 0; //used when doing drop or fit animations
	float fit_animation_frame_time_limit = 1.75f; //number of seconds to do the fit animation for
	float drop_animation_frame_time_limit = 1.5f; //number of seconds to do the drop animation for

	float m_fitSpeed = 3.0f;
	float m_dropSpeed = 6.0f;

	Transform light_tr;

	glm::vec4 cube_color = { 0.1, 0.9, 0.1, 1 };
	glm::vec4 wall_color = { 0.1, 0.2, 0.87, 1 };

	glm::vec3 m_camOffset = {-1, 4, 20};

	std::shared_ptr<OpenGLTexture2D> backgroundTexture;
	std::shared_ptr<OpenGLTexture2D> sandTexture;

	std::shared_ptr<Transform>  backgroundTransform;

	std::shared_ptr<OpenGLTexture2D> nightTexture;

	std::shared_ptr<Transform> m_worldTransform;

	std::vector<GameModel*> m_models;
	int m_currModel;

	enum class GameState { Spawn, Rotations, Advance, Fit, Drop, Debug, MainMenu };
	GameState m_state = GameState::Spawn;
	GameState m_lastState; //used when toggling debugmode

	const float m_debugToggleTimer = 0.2f;
	float m_currDebugToggleTimer = 0.0f;

	std::shared_ptr<Mesh> m_thinkerMesh;
	std::shared_ptr<Mesh> m_ColMesh;
	Transform m_thinkerTransform1 = Transform({ -6, -0.5f, -15 }, { -glm::radians(90.0f), 0, -glm::radians(75.0f) }, { 0.05f, 0.05f, 0.05f });
	Transform m_thinkerTransform2 = Transform({ 10, -0.5f, -15 }, { -glm::radians(90.0f), 0, -glm::radians(145.0f) }, { 0.05f, 0.05f, 0.05f });
	
	Transform m_colTransform1 = Transform({-50, -8.5f,-20 }, { -glm::radians(90.0f), 0, glm::radians(90.0f) }, { 0.3f, 0.3f, 0.3f });
	Transform m_colTransform2 = Transform({ 50, -8.5f,-20 }, { -glm::radians(90.0f), 0, glm::radians(90.0f) }, { 0.3f, 0.3f, 0.3f });
	Transform m_colTransform3 = Transform({-50, -8.5f, 20 }, { -glm::radians(90.0f), 0, glm::radians(90.0f) }, { 0.3f, 0.3f, 0.3f });
	Transform m_colTransform4 = Transform({ 50, -8.5f, 20 }, { -glm::radians(90.0f), 0, glm::radians(90.0f) }, { 0.3f, 0.3f, 0.3f });


	int m_streakNum = 0;
	float m_multiplicator = 1.0f;
	float m_speedUpFactor = 1.0f;
	float m_maxSpeedUpFactor = 6.5f;
	float m_maxMultiplicator = 8.0f;
	bool m_uiUpdated = false;
	
	void ResetSpeedAndMultiplicator()
	{
		m_multiplicator = 1.0f;
		m_speedUpFactor = 1.0f;
	}

	void UpdateSpeed()
	{
		if (m_speedUpFactor == m_maxSpeedUpFactor)
		{
			return;
		}

		m_speedUpFactor = 1.0f + 0.75f * (float)((int)m_streakNum / UPDATE_STREAK_FACTOR);

		if (m_speedUpFactor > m_maxSpeedUpFactor)
		{
			m_speedUpFactor = m_maxSpeedUpFactor;
		}
	}

	void UpdateMultiplicator()
	{
		if (m_multiplicator == m_maxMultiplicator)
		{
			return;
		}

		m_multiplicator = 1.0f + ((int)m_streakNum / UPDATE_STREAK_FACTOR);

		if (m_multiplicator > m_maxMultiplicator)
		{
			m_multiplicator = m_maxMultiplicator;
		}
	}

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
		score += m_models[m_currModel]->getScore() * m_multiplicator;
	}

	void UpdateGameTime()
	{
		m_gameTime += Time::GetDeltaTime();
	}

	void LoadGame()
	{
		//set font size
		ImGui::GetIO().FontGlobalScale = 1.5f;
		
		auto* controller = Application::GetCameraController();
		controller->setLookIsOn(false);
		controller->setMovementIsOn(false);
		controller->SetCursorControl(true);
		m_state = GameState::Spawn;
		Input::SetLockCursor(true);
	}

	void LoadMenu()
	{
		//set font size
		ImGui::GetIO().FontGlobalScale = 1.5f;

		auto* controller = Application::GetCameraController();
		controller->setLookIsOn(false);
		controller->setMovementIsOn(false);
		controller->SetCursorControl(false);
		m_state = GameState::MainMenu;
		Input::SetLockCursor(false);
	}

	void RenderGameUI()
	{
		int numExtraLines = 0;

		if (m_streakNum >= UPDATE_STREAK_FACTOR)
		{
			numExtraLines = 2;
		}

		ImGui::SetNextWindowPos(ImVec2{ 0, 0 });
		ImGui::SetNextWindowSize(ImVec2{ 150.0f + 20.0f * numExtraLines, 60.0f + 30.0f * numExtraLines });
		ImGui::Begin("Score");
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);

		ImGui::Text("Score %d", (int)score);

		if (numExtraLines > 0)
		{
			ImGui::Text("Combo %d", m_streakNum);
			ImGui::Text("Multiplier %dx", (int)m_multiplicator);
		}

		ImGui::End();

		GLFWwindow* w = Application::GetWindow();
		int width, height;
		glfwGetWindowSize(w, &width, &height);


		ImGui::SetNextWindowPos(ImVec2{ (float)width - 170, 0 });
		ImGui::SetNextWindowSize(ImVec2{ 170, 60 });
		ImGui::Begin("Time");
		ImGui::Text("Time %.2f", m_gameTime);
		ImGui::End();
	}

	void RenderMainMenu()
	{
		int width, height;
		glfwGetWindowSize(Application::GetWindow(), &width, &height);

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::Begin("Super Hyper Cube");

		if (ImGui::Button("Play", ImVec2(100, 50)))
		{
			LoadGame();
		}

		ImGui::SameLine();
		if (ImGui::Button("Exit", ImVec2(100, 50)))
		{
			Application::Exit();
		}

		ImGui::Image((void*)backgroundTexture->GetRendererID(), ImVec2(width, height - 150), 
			ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}
};