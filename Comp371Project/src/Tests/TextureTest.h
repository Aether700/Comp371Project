#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"

class TextureTest : public Script
{
public:
	void OnStart() 
	{
		m_cubeMap = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");
	}

	void OnRender()
	{
		Renderer3D::DrawVoxel(glm::mat4(1.0f));
		Renderer3D::DrawVoxel(m_c2.GetTransformMatrix(), m_cubeMap);
	}

private:
	std::shared_ptr<OpenGLCubeMap> m_cubeMap;
	Transform m_c1;
	Transform m_c2 = Transform({2, 0, 0});
};