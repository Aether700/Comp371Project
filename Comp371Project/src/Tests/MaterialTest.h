#pragma once
#include "../Rendering/Renderer3D.h"
#include "../Core/Script.h"

class MaterialTest : public Script
{
public:
	void OnStart()
	{
		Renderer3D::UseShadows(true);
		m_cubeTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");
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

		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			m_lightPos.x -= lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_lightPos.x += lightSpeed * Time::GetDeltaTime();
		}
	}

	void OnRender()
	{
		Renderer3D::DrawVoxel(m_cube.GetTransformMatrix(), glm::vec4{0, 1, 0, 1});
		Renderer3D::DrawVoxel(m_plane.GetTransformMatrix(), m_noShadowMat, m_cubeColor);
		Renderer3D::AddDirectionalLight(m_lightPos, m_cube.position - m_lightPos);
	}

private:
	glm::vec3 m_lightPos = {0, 3, -3};
	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	glm::vec4 m_cubeColor = { 1, 0, 1, 1 };
	std::shared_ptr<OpenGLCubeMap> m_cubeTexture;

	Material m_noShadowMat = Material(true);
};