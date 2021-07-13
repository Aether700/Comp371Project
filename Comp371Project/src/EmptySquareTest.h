#pragma once
#include "Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Core/Application.h"
#include "Core/Script.h"
#include "GraphicsAPI/OpenGLVertexArray.h"
#include "GraphicsAPI/OpenGLBuffer.h"

#include <memory>

class EmptySquareTest : public Script
{
public:
	void OnRender()
	{
		for (unsigned int x = 0; x < 5; x++)
		{
			for (unsigned int y = 0; y < 5; y++)
			{
				Renderer3D::DrawWireSquare({ x, y, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
			}
		}
		glm::vec4 color = { 1, 0, 0, 1 };

		if (Input::IsKeyPressed(GLFW_KEY_B))
		{
			color = { 0, 0, 1, 1 };
		}

		Renderer3D::DrawQuad(m_transform.GetTransformMatrix(), color);
		Renderer3D::DrawVoxel({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
	}

private:
	Transform m_transform = Transform(glm::vec3{ 3, 0, -2 });
	
};
