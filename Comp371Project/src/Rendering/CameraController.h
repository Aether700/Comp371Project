#pragma once
#include "../Core/Script.h"
#include "../Core/Application.h"
#include "../Core/Input.h"
#include "../Core/Time.h"
#include "Camera.h"

#include "../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"

class CameraController : public Script
{
public:
	void OnStart()
	{
		Input::SetLockCursor(true);
	}

	//handle camera input
	void OnUpdate()
	{
		Transform& camTransform = Application::GetCamera()->GetTransform();

		if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			Input::SetLockCursor(false);
		}

		if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			Input::SetLockCursor(true);
		}

		//handle camera movement
		if (Input::IsKeyPressed(GLFW_KEY_A))
		{
			camTransform.position.x -= m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_D))
		{
			camTransform.position.x += m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			camTransform.position.z += m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			camTransform.position.z -= m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		{
			camTransform.position.y += m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
		{
			camTransform.position.y -= m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		/*
		//handle the camera orientation
		glm::vec2 currMousePos = Input::GetMousePosition();

		if (m_lastMousePos != currMousePos)
		{
			glm::vec2 displacement = currMousePos - m_lastMousePos;

			camTransform.rotation.y += displacement.x * m_cameraRotationSpeed * Time::GetDeltaTime();
			camTransform.rotation.x += displacement.y * m_cameraRotationSpeed * Time::GetDeltaTime();

			if (camTransform.rotation.x > m_verticalClamp)
			{
				camTransform.rotation.x = m_cameraVerticalMoveSpeed;
			}
			else if(camTransform.rotation.x < -m_verticalClamp)
			{
				camTransform.rotation.x = -m_cameraVerticalMoveSpeed;
			}

			m_lastMousePos = currMousePos;
		}
		*/
		
		//pressing the home key resets the 
		//transform of the camera as specified in the assignment
		if (Input::IsKeyPressed(GLFW_KEY_HOME))
		{
			camTransform = Transform();
		}
	}

private:
	float m_cameraMoveSpeed = 5.0f;
	float m_cameraVerticalMoveSpeed = m_cameraMoveSpeed;
	float m_cameraRotationSpeed = 0.5f;

	float m_verticalClamp = glm::radians(90.0f);

	glm::vec2 m_lastMousePos;
};