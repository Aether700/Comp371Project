#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include "../Core/Script.h"
#include "../Core/Application.h"
#include "../Core/Input.h"
#include "../Core/Time.h"
#include "Camera.h"

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

//camera controller script responsible for the movement and rotation of the camera
class CameraController : public Script
{
public:
	void OnStart()
	{
		Input::SetLockCursor(true);
		m_lastMousePos = Input::GetMousePosition();

		m_defaultVerticalFOV = Application::GetCamera()->GetPerspectiveVerticalFOV();
	}

	/*handle camera input
	  we can use the wasd keys to move around and 
	  use the mouse to look around
		
	  the R key resets the camera transform and vertical FOV 
	  as required by the assignment
	*/
	void OnUpdate()
	{
		glm::mat4& camTransform = Application::GetCamera()->GetTransform();


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

		//handle the camera orientation

		//only try to rotate the camera if the mouse is locked
		if (m_mouseIsLocked)
		{
			glm::vec2 currMousePos = Input::GetMousePosition();

			//if mouse has not been moved don't do anything
			if (m_lastMousePos != currMousePos)
			{
				glm::vec2 displacement = currMousePos - m_lastMousePos;

				m_yRotation -= displacement.x * m_cameraRotationSpeed * Time::GetDeltaTime();
				m_upRotation -= displacement.y * m_cameraRotationSpeed * Time::GetDeltaTime();

				m_upRotation = Clamp(m_upRotation, -m_verticalClamp, m_verticalClamp);
				
				if (m_yRotation > 360)
				{
					m_yRotation -= 360;
				}
				else if (m_yRotation < 0)
				{
					m_yRotation += 360;
				}

				//convert rotations into radians
				float theta = glm::radians(m_yRotation);
				float phi = glm::radians(m_upRotation);

				//update look direction
				m_lookDir = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

				m_lastMousePos = currMousePos;
			}
		}

		//handle camera movement relative to the look direction of the camera
		if (Input::IsKeyPressed(GLFW_KEY_A))
		{
			glm::vec3 currLeft = -1.0f * glm::vec3{ glm::sin(glm::radians(m_yRotation)), 0,
				glm::cos(glm::radians(m_yRotation)) };
			m_camPos += currLeft * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_D))
		{
			glm::vec3 currRight = { glm::sin(glm::radians(m_yRotation)), 0, glm::cos(glm::radians(m_yRotation)) };
			m_camPos += currRight * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			glm::vec3 currBackward = { glm::sin(glm::radians(m_yRotation) - glm::radians(90.0f)), 0,
				glm::cos(glm::radians(m_yRotation) - glm::radians(90.0f)) };
			m_camPos += currBackward * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			glm::vec3 currForward = { glm::sin(glm::radians(m_yRotation) + glm::radians(90.0f)), 0,
				glm::cos(glm::radians(m_yRotation) + glm::radians(90.0f)) };
			m_camPos += currForward * m_cameraMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		{
			m_camPos.y += m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
		{
			m_camPos.y -= m_cameraVerticalMoveSpeed * Time::GetDeltaTime();
		}

		//pressing the R key resets the transform of the camera 
		//and it's vertical FOV as required
		if (Input::IsKeyPressed(GLFW_KEY_R))
		{
			ResetCamera();
		}
		
		//update camera transform
		camTransform = glm::lookAt(m_camPos, m_camPos + m_lookDir, { 0, 1, 0 });
	}

	/*handles scroll events and zooms in and out depending on the input provided
	  
	  allows the user to zoom in and out as required by the assignment
	*/
	virtual bool HandleScrollEvent(double xScroll, double yScroll) override
	{
		auto cam = Application::GetCamera();
		//only change the FOV if currently using perspective projection (for the assignment 
		//the camera will always remain in perspective projection)
		if (cam->GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			cam->SetPerspectiveVerticalFOV(yScroll * Time::GetDeltaTime() + cam->GetPerspectiveVerticalFOV());
		}

		return true;
	}

private:
	void ResetCamera()
	{
		Application::GetCamera()->SetPerspectiveVerticalFOV(m_defaultVerticalFOV);
		m_camPos = { 0, 0, 0 };
		m_yRotation = 90.0f;
		m_upRotation = 0.0f;
		m_lookDir = { 0, 0, -1 };
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}
		return value;
	}

	float m_cameraMoveSpeed = 15.0f;
	float m_cameraVerticalMoveSpeed = m_cameraMoveSpeed;
	float m_cameraRotationSpeed = 10.0f;

	float m_verticalClamp = 89.0f;
	
	bool m_mouseIsLocked = true;

	glm::vec2 m_lastMousePos;

	//zoom default settings
	float m_defaultVerticalFOV;
	
	float m_yRotation = 90.0f;
	float m_upRotation = 0.0f;

	glm::vec3 m_lookDir = { 0, 0, -1 };
	glm::vec3 m_camPos = { 0, 0, 0 };
};

#endif
