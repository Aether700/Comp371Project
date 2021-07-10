#pragma once
#include "Core/Script.h" 
#include "Rendering/Renderer3D.h"
#include "GraphicsAPI/OpenGLTexture2D.h"

class CubeRendereringTest : public Script
{
public:
	virtual void OnStart() override
	{
		m_transform.position.x = 5;
	}

	virtual void OnRender() override
	{
		Renderer3D::DrawVoxel(m_transform.GetTransformMatrix(), Renderer3D::GetDefaultWhiteCubeMap(), 1.0f, cubeColor);
	}

private:
	glm::vec4 cubeColor = { 1, 1, 1, 1 };
	Transform m_transform;
};