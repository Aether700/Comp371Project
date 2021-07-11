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

class CubeRendereringTest : public Script
{
public:
	
	void OnStart()
	{
		float position[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
		};

		m_vao = std::make_shared<OpenGLVertexArray>();
		m_vbo = std::make_shared<OpenGLVertexBuffer>(position, sizeof(position));

		m_vbo->SetLayout({ { ShaderDataType::Float3, "position" } });

		m_vao->AddVertexBuffer(m_vbo);

		unsigned int indices[] = {
			//back face
			0, 1, 2,
			2, 3, 0,

			//front face
			4, 5, 7,
			7, 6, 4,

			//left face
			0, 4, 6,
			6, 2, 0,

			//bottom face
			4, 5, 1,
			1, 0, 4,

			//right face
			5, 1, 3,
			3, 7, 5,

			//top face
			6, 7, 3,
			3, 2, 6
		};

		m_ibo = std::make_shared<OpenGLIndexBuffer>(indices, sizeof(indices) / sizeof(indices));
		
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
		Renderer3D::GetDefaultWhiteCubeMap()->Bind();
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