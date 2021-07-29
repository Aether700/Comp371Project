#ifndef _POINT_LIGHT_TEST_H_
#define _POINT_LIGHT_TEST_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include "../Core/Script.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLShader.h"

#include <sstream>

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/multiple_lights.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp

struct LightVertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	Material mat;
};

class PointLightTest : public Script
{
public:
	void OnStart()
	{
		PrepareCubeAndPlane();

		m_shader = std::make_shared<OpenGLShader>("/Users/madelainemareschal/Desktop/comp371project/comp371project/Resources/Shaders/MultipleLight.glsl");
		SetPointLight(m_shader, m_light.position, { 1, 1, 1, 1 }, 0);
		//SetPointLight(m_shader, {0, 5, -2}, { 0.25f, 0.5f, 1, 1 }, 1);
		m_shader->SetInt("u_numLights", 1);
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
			{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },

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

		constexpr unsigned int numVertices = sizeof(posAndNormals) / (sizeof(glm::vec3) * 2);
		LightVertexData cubePlanePos[3 * numVertices];

		unsigned int index = 0;
		//cube
		for (int i = 0; i < numVertices * 2; i+=2)
		{
			cubePlanePos[index].position = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = posAndNormals[i + 1];
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		//plane
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = posAndNormals[i + 1];
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = posAndNormals[i + 1];
			cubePlanePos[index].mat = m_defaultMat;
			index++;
		}
		m_vbo = std::make_shared<OpenGLVertexBuffer>((float*)(void*)cubePlanePos, sizeof(cubePlanePos));
		m_vbo->SetLayout({ 
			{ShaderDataType::Float3, "position"}, 
			{ShaderDataType::Float3, "normal"}, 
			{ShaderDataType::Float3, "ambiant"}, 
			{ShaderDataType::Float3, "diffuse"}, 
			{ShaderDataType::Float3, "specular"}, 
			{ShaderDataType::Float, "shininess"}
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
		unsigned int indexData[3 * indicesCount];

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < indicesCount; j++)
			{
				indexData[i * indicesCount + j] = indices[j] + (i * numVertices);
			}
		}

		m_ibo = std::make_shared<OpenGLIndexBuffer>(indexData, sizeof(indexData) / sizeof(unsigned int));
	}

	void SetPointLight(std::shared_ptr<OpenGLShader>& shader, glm::vec3 position, glm::vec4 color, int index)
	{
		shader->Bind();
		std::stringstream ss;
		ss << "u_pointLights[" << index << "].";
		

		shader->SetFloat3((ss.str() + "position").c_str(), position);
		shader->SetFloat4((ss.str() + "color").c_str(), color);
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLVertexBuffer> m_lights;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	Transform m_light = Transform({ 2, 3, -6 }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});

	Material m_defaultMat = { {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 1}, 32 };
	Material m_testMat = { {0.1f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.75f, 0, 0}, 32 };
};

#endif
