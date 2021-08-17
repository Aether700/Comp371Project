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


		if (m_hasCursorControl)
		{
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
		}
		//handle the camera orientation

		//only try to rotate the camera if the mouse is locked and looking around is enabled
		if (m_mouseIsLocked && lookIsOn)
		{
			glm::vec2 currMousePos = Input::GetMousePosition();

			//if mouse has not been moved don't do anything
			if (m_lastMousePos != currMousePos)
			{
				glm::vec2 displacement = currMousePos - m_lastMousePos;


				ApplyLookDisplacement(displacement);
				m_lastMousePos = currMousePos;
			}
		}

		if(movementIsOn)
		{
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

			
		}

		//update camera transform
		camTransform = glm::lookAt(m_camPos, m_camPos + m_lookDir, m_camUp);
	}

	glm::vec3 GetCamPos() const { return m_camPos; }

	/*handles scroll events and zooms in and out depending on the input provided
	  
	  allows the user to zoom in and out as required by the assignment
	*/
	virtual bool HandleScrollEvent(double xScroll, double yScroll) override
	{
		//only process scroll events when the camera can look around and move
		if (!lookIsOn || !movementIsOn)
		{
			return false;
		}

		auto cam = Application::GetCamera();
		//only change the FOV if currently using perspective projection (for the assignment 
		//the camera will always remain in perspective projection)
		if (cam->GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			cam->SetPerspectiveVerticalFOV(yScroll * Time::GetDeltaTime() + cam->GetPerspectiveVerticalFOV());
		}

		return true;
	}

	//TODO: this function is not great
	void SetCamera(glm::vec3 pos, glm::vec3 lookat, float y_rot, float up_rot)
	{
		Application::GetCamera()->SetPerspectiveVerticalFOV(m_defaultVerticalFOV);
		m_camPos = pos;
		m_yRotation = y_rot;
		m_upRotation = up_rot;
		m_lookDir = (lookat - pos);

		//Method of setting camera using camera position + given y / up angles:
		/* The application has to be manually nudged into showing the right lookAt direction
		* Otherwise, it looks in odd directions but suddenly snaps to the right one the moment the mouse moves at all (And the code below is run as part of the mouse update calculations)
		*
		* To use pos+lookat instead, we need to figure out what yRotation and upRotation should be, not because we need them, but because the camera will jump on the first mouse movement if they are wrong
		*/

		// Camera position + give angles method:

		//convert rotations into radians
		//float theta = glm::radians(m_yRotation);
		//float phi = glm::radians(m_upRotation);

		////update look direction
		//m_lookDir = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));


		// Camera position + lookat point method:
		glm::vec3 norm_look = glm::normalize(m_lookDir);

		// y rotation: Rotation around y-axis to look left/right
		m_yRotation = glm::degrees(glm::acos(glm::dot(norm_look, glm::vec3({ 1,0,0 }))));
		if (m_lookDir.z > 0) { m_yRotation *= -1.0f; } //the angle calculated is right, but if we are now 'behind' the object, need to apply it in the opposite rotation direction

		// up rotation: based on change in y and xz from camera position to lookat position
		m_upRotation = -90.0f + glm::degrees(glm::acos(glm::dot(norm_look, glm::vec3({ 0,-1,0 }))));

		//std::cout << "yRot: " << m_yRotation << std::endl;
		//std::cout << "upRot: " << m_upRotation << std::endl;

		OnUpdate();

	}

	void setMovementIsOn(bool s)
	{
		movementIsOn = s;
	}

	void setLookIsOn(bool s)
	{
		lookIsOn = s;
	}

	void SetCursorControl(bool value) { m_hasCursorControl = value; }

private:

	//common function for mouse and keyboard (single axis) to change look direction
	void ApplyLookDisplacement(glm::vec2 displacement)
	{
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
	glm::vec3 m_camUp = { 0, 1, 0 };

	bool movementIsOn = true;
	bool lookIsOn = true;
	bool m_hasCursorControl = true; //tell the controller whether to control the freeing and locking of the cursor
};