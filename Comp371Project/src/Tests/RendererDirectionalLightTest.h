#pragma once
#include "../Rendering/Renderer3D.h"
#include "../Core/Script.h"

class RendererDirectionalLightTest : public Script
{
public:
	void OnStart()
	{
		Application::SetBackgroundColor({ 1, 1, 1, 1 });
		Renderer3D::UseShadows(true);
	}

	void OnUpdate()
	{
		float lightSpeed = 5.0f;

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			m_lightPos.z -= lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			m_lightPos.z += lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_lightPos.x += lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			m_lightPos.x -= lightSpeed * Time::GetDeltaTime();
		}
	}

	void OnRender()
	{
		Renderer3D::DrawVoxel(m_cube.GetTransformMatrix(), m_geometryColor);
		Renderer3D::DrawVoxel(m_plane.GetTransformMatrix(), m_geometryColor);
		//Renderer3D::AddDirectionalLight(m_staticLight, {0, -1, 0});
		Renderer3D::AddDirectionalLight(m_lightPos, { -0.5, -1, 0 });
		//std::cout << "light pos = " << m_lightPos.x << ", " << m_lightPos.y << ", " << m_lightPos.z << "\n";
	}

private:
	glm::vec4 m_geometryColor = {0.3, 0.3, 0.85, 1 };

	glm::vec3 m_staticLight = { 3, 1, -3};

	glm::vec3 m_lightPos = { -5, 4.0f, -3 };//{ 2.0f, 2.0f, -6.0f };
	Transform m_cube = Transform({ 0, 0, -3 }, { 0, 0, 0 }, {1, 4, 5});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, { 100, 0.5f, 100 });
};