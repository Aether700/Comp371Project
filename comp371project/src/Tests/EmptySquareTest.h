#ifndef _EMPTY_SQUARE_TEST_H_
#define _EMPTY_SQUARE_TEST_H_

#include <GL/glew.h>
#include "../Core/Application.h"
#include "../Core/Input.h"
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLBuffer.h"

#include <memory>

class EmptySquareTest : public Script
{
public:
	void OnRender()
	{
		/*
		for (int x = -50; x < 50; x++) // started at -50 since it need to be center at origin assuming(0,0) being origin
		{
			for (int z = -50; z < 50; z++)
			{
				Renderer3D::DrawWireSquare({ x, -0.5, z }, { 1.5708, 0, 0 }, { 1, 1, 1 }); 
				//1.5708rad= 90degree since size of the square is 1 and by rotating it 90 degree it will be at heigh (1,0.5,1) whihc is why the (x,-0.5, z)
			}
		}
		*/
		
		
		glm::vec4 color = { 1, 0, 0, 1 };

		if (Input::IsKeyPressed(GLFW_KEY_B))
		{
			color = { 0, 0, 1, 1 };
		}

		Renderer3D::DrawQuad(m_transform.GetTransformMatrix(), color);
		//Renderer3D::DrawVoxel({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		
	}

private:
	Transform m_transform = Transform({ 0, 0, 0 }, { glm::radians(90.0f), 0, 0 }, {1, 1, 1});
	
};

#endif
