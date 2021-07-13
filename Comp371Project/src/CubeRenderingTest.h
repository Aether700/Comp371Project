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


#include <sstream>
#include <string>
#include <assert.h>

struct VertexCubeData
{
	glm::vec3 pos;
};

//https://github.com/PancakeAssassin/OpenGLTutorialRepository/blob/master/OpenGLTutorial2/main.cpp

class CubeRendereringTest : public Script
{
public:
	
	void OnStart()
	{
		float cubeVertices[] =
		{
			 0.5f,  0.5f,  0.5f, //0
			-0.5f,  0.5f,  0.5f, //1
			-0.5f,  0.5f, -0.5f, //2
			 0.5f,  0.5f, -0.5f, //3
			 0.5f, -0.5f,  0.5f, //4
			-0.5f, -0.5f,  0.5f, //5
			-0.5f, -0.5f, -0.5f, //6
			 0.5f, -0.5f, -0.5f  //7
		};

		m_vao = std::make_shared<OpenGLVertexArray>();
		m_vbo = std::make_shared<OpenGLVertexBuffer>(cubeVertices, sizeof(cubeVertices));

		m_vbo->SetLayout({ { ShaderDataType::Float3, "position" } });

		m_vao->AddVertexBuffer(m_vbo);

		unsigned int indices[] =
		{
			//top face
			0, 1, 3, 
			3, 1, 2,


			//front face
			2, 6, 7, 
			7, 3, 2,
			
			//bottom face
			7, 6, 5, 
			5, 4, 7,

			//back face
			5, 1, 4, 
			4, 1, 0,

			//right face
			4, 3, 7, 
			3, 4, 0,

			//left face
			5, 6, 2,
			5, 1, 2
		};
		m_ibo = std::make_shared<OpenGLIndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
		
		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/CubeShader.glsl");

		/*
		Application::GetCamera()->GetTransform().position.z = 5.0f;

		//create vertex array
		m_vao = std::make_shared<OpenGLVertexArray>();


		float pos[] = {
			 0.0f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f
		};

		m_vbo = std::make_shared<OpenGLVertexBuffer>(pos, sizeof(pos));
		m_vbo->SetLayout({ { ShaderDataType::Float3, "position" } });

		m_vao->AddVertexBuffer(m_vbo);

		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/CubeShader.glsl");
		*/
	}

	void OnRender()
	{
		glm::mat4 viewProjMatrix = Application::GetCamera()->GetProjectionMatrix()
			* glm::inverse(Application::GetCamera()->GetTransform().GetTransformMatrix());

		m_shader->Bind();
		m_shader->SetMat4("u_viewProjMatrix", viewProjMatrix);
		
		
		//bind vertex array and vertex buffer before drawing since 
		//other stuff might have been bound before this is called
		//Renderer3D::GetDefaultWhiteCubeMap()->Bind();
		m_vao->Bind();
		m_vbo->Bind();
		m_ibo->Bind();

		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		
		
		//Renderer3D::DrawVoxel(m_transform.GetTransformMatrix(), Renderer3D::GetDefaultWhiteCubeMap());
	}

private:
	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;

	//old stuff
	glm::vec4 cubeColor = { 1, 1, 1, 1 };
	Transform m_transform;
};