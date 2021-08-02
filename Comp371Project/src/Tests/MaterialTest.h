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

	void OnRender()
	{
		Renderer3D::DrawVoxel(m_cube.GetTransformMatrix(), m_cubeTexture, 1, m_cubeColor);
	}

private:
	Transform m_cube;
	glm::vec4 m_cubeColor = { 1, 0, 1, 1 };
	std::shared_ptr<OpenGLCubeMap> m_cubeTexture;
};