#pragma once
#define GLEW_STATIC
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Core/Script.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLShader.h"

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/2.2.basic_lighting_specular/basic_lighting_specular.cpp

class PointLightTest : public Script
{
public:
	void OnStart()
	{
		PrepareCubeAndPlane();

		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShader.glsl");
		m_shader->Bind();
		m_shader->SetFloat3("u_lightPos", m_lightPos);
		/*
		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/TriangleInWorldSpace.glsl");
		*/
	}

	void OnRender()
	{
		auto camera = Application::GetCamera();
		glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetTransform();

		m_shader->Bind();
		m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

private:

	void PrepareCubeAndPlane()
	{
		m_vao = std::make_shared<OpenGLVertexArray>();
		/*//back face
		1, 0, 2,
		2, 3, 1,

		//left face
		0, 4, 6,
		6, 2, 0,

		//front face
		4, 5, 7, 
		7, 6, 4,

		//right face
		5, 1, 3,
		3, 7, 5,

		//top face
		6, 7, 3, 
		3, 2, 6,

		//bottom face
		0, 1, 5,
		5, 4, 0
		*/

		glm::vec3 posAndNormals[] = {
			//back face
			{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, //0
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },	//1
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },	//2
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },	//3

			//left face
			{ -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },

			//front face
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },

			//right face
			{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },

			//top face
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },

			//bottom face
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }

			/* actual vertices
			{ -0.5f, -0.5f, -0.5f }, //0
			{  0.5f, -0.5f, -0.5f }, //1
			{ -0.5f,  0.5f, -0.5f }, //2
			{  0.5f,  0.5f, -0.5f }, //3
			{ -0.5f, -0.5f,  0.5f }, //4
			{  0.5f, -0.5f,  0.5f }, //5
			{ -0.5f,  0.5f,  0.5f }, //6
			{  0.5f,  0.5f,  0.5f }, //7
			*/
		};

		constexpr unsigned int numVectors = sizeof(posAndNormals) / sizeof(glm::vec3);
		glm::vec3 cubePlanePos[2 * numVectors];

		//cube
		for (int i = 0; i < numVectors; i+=2)
		{
			cubePlanePos[i] = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[i + 1] = posAndNormals[i + 1];
		}

		//plane
		for (int i = 0; i < numVectors; i += 2)
		{
			cubePlanePos[numVectors + i] = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[numVectors + i + 1] = posAndNormals[i + 1];
		}

		m_vbo = std::make_shared<OpenGLVertexBuffer>(&(cubePlanePos[0].x), sizeof(cubePlanePos));
		m_vbo->SetLayout({ 
			{ShaderDataType::Float3, "position"}, 
			{ShaderDataType::Float3, "normal"}, 
		});

		m_vao->AddVertexBuffer(m_vbo);

		unsigned int indices[] = {
			//back face
			0, 1, 2,
			2, 3, 0,

			//left face
			4, 5, 6,
			6, 7, 4,

			//front face
			8, 9, 10,
			10, 11, 8,

			//right face
			12, 13, 14,
			14, 15, 12,

			//top face
			16, 17, 18,
			18, 19, 16,

			//bottom face
			20, 21, 22,
			22, 23, 20
		};

		constexpr unsigned int indicesCount = sizeof(indices) / sizeof(unsigned int);
		unsigned int indexData[2 * indicesCount];

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < indicesCount; j++)
			{
				indexData[i * indicesCount + j] = indices[j] + (i * (numVectors / 2));
			}
		}

		m_ibo = std::make_shared<OpenGLIndexBuffer>(indexData, sizeof(indexData) / sizeof(unsigned int));
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});

	glm::vec3 m_lightPos = {2, 3, -6};
};