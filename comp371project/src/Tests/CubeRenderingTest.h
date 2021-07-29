#ifndef _CUBE_RENDERING_TEST_H_
#define _CUBE_RENDERING_TEST_H_

#define GLEW_STATIC
#include "../Core/Script.h" 
#include "../Core/Application.h" 
#include "../Rendering/Renderer3D.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../GraphicsAPI/OpenGLShader.h"
#include "../Core/Debug.h"
#include "../Core/Input.h"


#include <sstream>
#include <string>
#include <assert.h>

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
		
		Renderer3D::DrawQuad(m_transform.GetTransformMatrix(), color);
		Renderer3D::DrawVoxel({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
	}

private:
	Transform m_transform = Transform(glm::vec3{3, 0, -2});
};


#endif
