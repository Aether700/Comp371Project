#pragma once
#define GLEW_STATIC
#include "Core/Script.h" 
#include "Core/Application.h" 
#include "Rendering/Renderer3D.h"
#include "GraphicsAPI/OpenGLTexture2D.h"
#include "Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "GraphicsAPI/OpenGLShader.h"
#include "Core/Debug.h"
#include "Core/Input.h"


#include <sstream>
#include <string>
#include <assert.h>

//https://github.com/PancakeAssassin/OpenGLTutorialRepository/blob/master/OpenGLTutorial2/main.cpp

class CubeRendereringTest : public Script
{
public:
	void OnRender()
	{
		glm::vec4 color = { 1, 0, 0, 1 };

		if (Input::IsKeyPressed(GLFW_KEY_B))
		{
			color = { 0, 0, 1, 1 };
		}
		
		Renderer3D::DrawVoxel(m_transform.GetTransformMatrix(), Renderer3D::GetDefaultWhiteCubeMap(), GL_TRIANGLES, 1, color);
	}

private:
	Transform m_transform;
};