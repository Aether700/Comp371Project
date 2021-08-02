#pragma once
#define GLEW_STATIC
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Core/Script.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLFramebuffer.h"
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../Rendering/CameraController.h"
#include "../Rendering/Renderer3D.h"

#include <vector>
#include <sstream>

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/multiple_lights.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/point_shadows_soft.cpp

struct LightVertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

struct PointLight
{
	Transform transform;
	float farPlane;
	float nearPlane;

	PointLight(glm::vec3 pos, float far, float near) 
		: transform(pos), farPlane(far), nearPlane(near) { }
};

class AlecPointLightTest : public Script
{
private:
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

public:
	void OnStart()
	{
		Application::SetBackgroundColor({ 1, 1, 1, 1 });
		PrepareCubeAndPlane();

		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShader.glsl");
		m_lightShader = std::make_shared<OpenGLShader>("Resources/Shaders/LightPos.glsl");
		m_shadowMapShader = std::make_shared<OpenGLShader>("Resources/Shaders/ShadowMap.glsl");
		PrepareShadowMap();
		//SetPointLight(m_shader, m_light.position, {1, 1, 1, 1}, 0);

		m_shader->SetInt("u_shadowMap", 0);

		
	}

	
	void OnUpdate()
	{
		float lightSpeed = 5.0f;

		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			m_light.transform.position.z -= lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			m_light.transform.position.z += lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_light.transform.position.x += lightSpeed * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			m_light.transform.position.x -= lightSpeed * Time::GetDeltaTime();
		}
	}


	void OnRender()
	{
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);

		GenerateShadowMap();
		
		// reset viewport
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		auto camera = Application::GetCamera();
		glm::mat4 camTransform = camera->GetTransform();
		glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camTransform;

		m_shader->Bind();

		m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		m_shader->SetFloat3("u_viewPos", Application::GetCameraController()->GetCamPos());
		m_shader->SetFloat3("u_lightPos", m_light.transform.position);
		//m_shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);
		m_shader->SetFloat("u_farPlane", m_light.farPlane);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		//m_lightShader->Bind();
		//m_lightShader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		//RenderLight();

		/*
		////render Depth map to quad for visual debugging
		////---------------------------------------------
		m_shadowMapDebugShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();
		*/



	}

private:

	void PrintMat4(const glm::mat4& m)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				std::cout << m[j][i] << " ";
			}
			std::cout << "\n";
		}
	}

	void RenderLight()
	{
		m_lightShader->Bind();
		m_vaoLight->Bind();
		m_iboLight->Bind();
		glDrawElements(GL_TRIANGLES, m_iboLight->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

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

		glm::vec2 textureCoords[] = {
			{0, 0},
			{1, 0},
			{0, 1},
			{1, 1}
		};

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
		LightVertexData cubePlanePos[2 * numVertices];
		LightVertexData lightPosAndNormal[numVertices];

		unsigned int index = 0;
		//cube
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
			cubePlanePos[index].textureCoords = textureCoords[i % 4];
			index++;
		}


		//plane
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
			index++;
		}

		index = 0;

		//light
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			lightPosAndNormal[index].position = m_light.transform.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			lightPosAndNormal[index].normal = m_light.transform.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0);
			index++;
		}

		m_vbo = std::make_shared<OpenGLVertexBuffer>((float*)(void*)cubePlanePos, sizeof(cubePlanePos));
		m_vbo->SetLayout({
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float3, "normal"},
			{ShaderDataType::Float2, "textureCoords"},
			});

		m_vboLight = std::make_shared<OpenGLVertexBuffer>((float*)(void*)lightPosAndNormal, sizeof(lightPosAndNormal));
		m_vboLight->SetLayout({
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float3, "normal"},
			{ShaderDataType::Float2, "textureCoords"},
			});

		m_vao->AddVertexBuffer(m_vbo);

		m_vaoLight = std::make_shared<OpenGLVertexArray>();
		m_vaoLight->AddVertexBuffer(m_vboLight);

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
		unsigned int lightIndices[indicesCount];

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < indicesCount; j++)
			{
				indexData[i * indicesCount + j] = indices[j] + (i * numVertices);
			}
		}

		for (int j = 0; j < indicesCount; j++)
		{
			lightIndices[j] = indices[j];
		}

		m_ibo = std::make_shared<OpenGLIndexBuffer>(indexData, sizeof(indexData) / sizeof(unsigned int));
		m_iboLight = std::make_shared<OpenGLIndexBuffer>(lightIndices, sizeof(lightIndices) / sizeof(unsigned int));
	}

	void SetPointLight(std::shared_ptr<OpenGLShader>& shader, glm::vec3 position, glm::vec4 color, int index)
	{
		shader->Bind();
		std::stringstream ss;
		ss << "u_pointLights[" << index << "].";


		shader->SetFloat3((ss.str() + "position").c_str(), position);
		shader->SetFloat4((ss.str() + "color").c_str(), color);
	}

	void GenerateShadowMap()
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_HEIGHT / (float)SHADOW_HEIGHT,
			m_light.nearPlane, m_light.farPlane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.transform.position, m_light.transform.position
			+ glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_shadowMapShader->Bind();
		for (unsigned int i = 0; i < 6; ++i)
			m_shadowMapShader->SetMat4("u_lightSpaceMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		m_shadowMapShader->SetFloat("u_farPlane", m_light.farPlane);
		m_shadowMapShader->SetFloat3("u_lightPos", m_light.transform.position);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PrepareShadowMap()
	{
		glGenFramebuffers(1, &m_fbo);
		// create depth cubemap texture
		glGenTextures(1, &m_cubemap);
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_cubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;
	std::shared_ptr<OpenGLShader> m_lightShader;
	std::shared_ptr<OpenGLShader> m_shadowMapShader;
	std::shared_ptr<OpenGLShader> m_shadowMapDebugShader;

	std::shared_ptr<OpenGLVertexArray> m_vaoLight;
	std::shared_ptr<OpenGLVertexBuffer> m_vboLight;
	std::shared_ptr<OpenGLIndexBuffer> m_iboLight;

	std::shared_ptr<OpenGLFramebuffer> m_framebuffer;

	glm::mat4 m_lightSpaceMatrix;

	Transform m_cube = Transform({ 0, 0, -3 });
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, { 10, 0.5f, 10 });
	PointLight m_light = PointLight({ 0, 2, -3.0f }, 1, 25);

	//temp
	unsigned int m_fbo;
	unsigned int m_cubemap;
	CameraController* m_camController;
};