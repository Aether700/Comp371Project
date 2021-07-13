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
	//handle camera input
	void OnUpdate()
	{
		Transform& camTransform = Application::GetCamera()->GetTransform();
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

		//handles the rotation from the arrow keys
		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			camTransform.rotation.x += m_cameraRotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			camTransform.rotation.x -= m_cameraRotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			camTransform.rotation.z += m_cameraRotationSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			camTransform.rotation.z -= m_cameraRotationSpeed * Time::GetDeltaTime();
		}

		//pressing the home key resets the 
		//transform of the camera as specified in the assignment
		if (Input::IsKeyPressed(GLFW_KEY_HOME))
		{
			camTransform = Transform();
		}

		//takes care of panning when right click is held
		if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			glm::vec2 currMousePos = Input::GetMousePosition();
			if (!m_rightClickPressed)
			{
				m_rightClickPressed = true;
			}
			else
			{
				glm::vec2 displacementVector = currMousePos - m_lastMousePos;
				camTransform.position.x += displacementVector.x * m_panSpeed * Time::GetDeltaTime();
			}

			m_lastMousePos = currMousePos;
		}
		else
		{
			m_rightClickPressed = false;
		}

		//tilting here
	}

private:
	float m_cameraMoveSpeed = 5.0f;
	float m_cameraRotationSpeed = 3.0f;

	float m_panSpeed = 0.5f;

	glm::vec2 m_lastMousePos;
	bool m_rightClickPressed;
};