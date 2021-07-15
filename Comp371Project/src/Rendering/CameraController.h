#pragma once
#include "../Core/Script.h"
#include "../Core/Application.h"
#include "../Core/Input.h"
#include "../Core/Time.h"
#include "Camera.h"

#include "../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"

//camera controller script responsible for the movement and rotation of the camera
class CameraController : public Script
{
public:
	void OnStart()
	{
		Input::SetLockCursor(true);
		m_lastMousePos = Input::GetMousePosition();
	}

	//handle camera input
	void OnUpdate()
	{
		Transform& camTransform = Application::GetCamera()->GetTransform();

		//manage the logic to lock and free the cursor
		if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			Input::SetLockCursor(false);
			m_mouseIsLocked = false;
		}

		if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			Input::SetLockCursor(true);
			m_mouseIsLocked = true;
			
			//reset last mouse pos to avoid gittery movement
			m_lastMousePos = Input::GetMousePosition();
		}

		//handle camera movement relative to the look direction of the camera
		if (Input::IsKeyPressed(GLFW_KEY_A))
		{
			float& rotation = camTransform.rotation.y;
			glm::vec3 currLeft = { glm::sin(rotation - glm::radians(90.0f)), 0, glm::cos(rotation - glm::radians(90.0f)) };
			camTransform.position += currLeft * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_D))
		{
			float& rotation = camTransform.rotation.y;
			glm::vec3 currRight = { glm::sin(rotation + glm::radians(90.0f)), 0, glm::cos(rotation + glm::radians(90.0f)) };
			camTransform.position += currRight * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			float& rotation = camTransform.rotation.y;
			glm::vec3 currBackward = { glm::sin(rotation), 0, glm::cos(rotation) };
			camTransform.position += currBackward * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			float& rotation = camTransform.rotation.y;
			glm::vec3 currForward = -1.0f * glm::vec3{ glm::sin(rotation), 0, glm::cos(rotation) };
			camTransform.position += currForward * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		{
			camTransform.position.y += m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
		{
			camTransform.position.y -= m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		//handle the camera orientation
		
		//only try to rotate the camera if the mouse is locked
		if (m_mouseIsLocked)
		{
			glm::vec2 currMousePos = Input::GetMousePosition();

			//if mouse has not been move don't do anything
			if (m_lastMousePos != currMousePos)
			{
				glm::vec2 displacement = currMousePos - m_lastMousePos;
				//glm::lookAt(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));

				camTransform.rotation.y -= displacement.x * m_cameraRotationSpeed * Time::GetDeltaTime();
				
				camTransform.rotation.x -= glm::cos(camTransform.rotation.y) * displacement.y 
					* m_cameraRotationSpeed * Time::GetDeltaTime();

				camTransform.rotation.z -= glm::sin(camTransform.rotation.y) * displacement.y
					* m_cameraRotationSpeed * Time::GetDeltaTime();

				/*
				if (camTransform.rotation.x > m_verticalClamp)
				{
					camTransform.rotation.x = m_verticalClamp;
				}
				else if (camTransform.rotation.x < -m_verticalClamp)
				{
					camTransform.rotation.x = -m_verticalClamp;
				}
				
				if (camTransform.rotation.z > m_verticalClamp)
				{
					camTransform.rotation.z = m_verticalClamp;
				}
				else if (camTransform.rotation.z < -m_verticalClamp)
				{
					camTransform.rotation.z = -m_verticalClamp;
				}
				*/

				m_lastMousePos = currMousePos;
			}
		}
		
		//pressing the R key resets the transform of the camera
		if (Input::IsKeyPressed(GLFW_KEY_R))
		{
			camTransform = Transform();
		}
	}

private:
	float m_cameraMoveSpeed = 5.0f;
	float m_cameraVerticalMoveSpeed = m_cameraMoveSpeed;
	float m_cameraRotationSpeed = 0.2f;

	float m_verticalClamp = glm::radians(90.0f);

	bool m_mouseIsLocked = true;

	glm::vec2 m_lastMousePos;
};