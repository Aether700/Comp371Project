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
	}
};
