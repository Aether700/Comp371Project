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

				float& rotation = camTransform.rotation.y;
				float forwardZValue = -1.0f * glm::cos(camTransform.rotation.y);

				float xRotationChange = displacement.y * m_cameraRotationSpeed * Time::GetDeltaTime();

				//make sure the rotation of the camera is relative to it's look direction
				if (forwardZValue < 0.0f)
				{
					xRotationChange *= -1.0f;
				}

				camTransform.rotation.y -= displacement.x * m_cameraRotationSpeed * Time::GetDeltaTime();
				camTransform.rotation.x += xRotationChange;

				if (camTransform.rotation.x > m_verticalClamp)
				{
					camTransform.rotation.x = m_verticalClamp;
				}
				else if (camTransform.rotation.x < -m_verticalClamp)
				{
					camTransform.rotation.x = -m_verticalClamp;
				}

				m_lastMousePos = currMousePos;
			}
		}
		
		//pressing the home key resets the 
		//transform of the camera as specified in the assignment
		if (Input::IsKeyPressed(GLFW_KEY_HOME))
		{
			camTransform = Transform();
		}
	}

private:
	/*
	//the components of direction should be either -1, 0 or 1
	void MoveCamera(Transform& camTransform, const glm::vec3& direction)
	{

		float horizontal = glm::cos(camTransform.rotation.y) * m_cameraMoveSpeed;
		camTransform.position.x += glm::cos(yaw) * horizontal * direction.x * Time::GetDeltaTime();
		camTransform.position.z -= glm::sin(yaw) * horizontal * direction.z * Time::GetDeltaTime();
		camTransform.position.y += glm::sin(camTransform.rotation.y) * m_cameraVerticalMoveSpeed * direction.y * Time::GetDeltaTime();
		//if you wanted strafing, you need this
		x -= glm::sin(camTransform.rotation.y) * m_cameraMoveSpeed;
		z += glm::cos(camTransform.rotation.y) * m_cameraMoveSpeed;

	}
	*/

	float m_cameraMoveSpeed = 5.0f;
	float m_cameraVerticalMoveSpeed = m_cameraMoveSpeed;
	float m_cameraRotationSpeed = 0.2f;

	float m_verticalClamp = glm::radians(90.0f);

	bool m_mouseIsLocked = true;

	glm::vec2 m_lastMousePos;
};