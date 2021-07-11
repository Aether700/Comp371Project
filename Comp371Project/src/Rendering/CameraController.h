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
	}

private:
	float m_cameraMoveSpeed = 5.0f;

};