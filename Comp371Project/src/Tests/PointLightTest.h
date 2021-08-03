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

#include <vector>
#include <sstream>

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/multiple_lights.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/point_shadows_soft.cpp

struct LightVertexData
{
	glm::vec3 position;
	glm::vec3 normal;
};

class PointLightTest : public Script
{
public:
	void OnStart()
	{
		Application::SetBackgroundColor({ 0, 0, 0, 1 });
		Renderer3D::UseShadows(true);
		PrepareCubeAndPlane();
			
		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShader.glsl");
		m_shadowMapShader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShadowMap.glsl");	
		PrepareShadowMap();

		m_shader->Bind();
		m_shader->SetInt("u_shadowMap", 0);
	}

	void OnUpdate()
	{
		if (Input::IsKeyPressed(GLFW_KEY_UP))
		{
			m_light.position.z -= 5*Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
		{
			m_light.position.z += 5*Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
		{
			m_light.position.x -= 5 * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			m_light.position.x += 5 * Time::GetDeltaTime();
		}

	}


	void OnRender()
	{
		Renderer3D::DrawVoxel(m_cube.GetTransformMatrix(), glm::vec4{ 0.5, 0, 0.5, 1 });
		Renderer3D::DrawVoxel(m_plane.GetTransformMatrix(), glm::vec4{0.3, 0.7, 0.1, 1 });
		Renderer3D::AddPointLight(m_light.position);

		
		//GenerateShadowMap();

		////clean up
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//int width, height;
		//glfwGetWindowSize(Application::GetWindow(), &width, &height);
		//glViewport(0, 0, width, height);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_shader->Bind();
		//auto camera = Application::GetCamera();
		//glm::mat4 camTransform = camera->GetTransform();
		//glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camTransform;

		//m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		//
		//// set lighting uniforms
		//m_shader->SetFloat3("u_lightPos", m_light.position);
		//m_shader->SetFloat3("u_viewPos", Application::GetCameraController()->GetCamPos());
		//m_shader->SetFloat("u_farPlane", m_farPlane);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMapID);
		//RenderScene();
		
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
		LightVertexData cubePlanePos[2 * numVertices];
		LightVertexData lightPosAndNormal[numVertices];

		unsigned int index = 0;
		//cube
		for (int i = 0; i < numVertices * 2; i+=2)
		{
			cubePlanePos[index].position = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
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
			lightPosAndNormal[index].position = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			lightPosAndNormal[index].normal = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0);
			index++;
		}

		m_vbo = std::make_shared<OpenGLVertexBuffer>((float*)(void*)cubePlanePos, sizeof(cubePlanePos));
		m_vbo->SetLayout({ 
			{ShaderDataType::Float3, "position"}, 
			{ShaderDataType::Float3, "normal"}
		});

		m_vboLight = std::make_shared<OpenGLVertexBuffer>((float*)(void*)lightPosAndNormal, sizeof(lightPosAndNormal));
		m_vboLight->SetLayout({
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float3, "normal"}
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

	void PrepareShadowMap()
	{
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		// create depth cubemap texture
		unsigned int depthCubemap;
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				m_fboSize, m_fboSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_fbo = depthMapFBO;
		m_shadowMapID = depthCubemap;
	}
	
	void RenderScene()
	{
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void GenerateShadowMap()
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 
			(float)m_fboSize / (float)m_fboSize, m_nearPlane, m_farPlane);
		
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position
			+ glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		//render scene to depth cubemap
		glViewport(0, 0, m_fboSize, m_fboSize);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_shadowMapShader->Bind();
		for (unsigned int i = 0; i < 6; ++i)
			m_shadowMapShader->SetMat4("u_lightSpaceMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		m_shadowMapShader->SetFloat("u_farPlane", m_farPlane);
		m_shadowMapShader->SetFloat3("u_lightPos", m_light.position);
		RenderScene();
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;
	std::shared_ptr<OpenGLShader> m_shadowMapShader;

	std::shared_ptr<OpenGLVertexArray> m_vaoLight;
	std::shared_ptr<OpenGLVertexBuffer> m_vboLight;
	std::shared_ptr<OpenGLIndexBuffer> m_iboLight;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	Transform m_light = Transform({ 0, 3, -5 }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});

	unsigned int m_fbo;
	unsigned int m_fboSize = 1024;
	unsigned int m_shadowMapID;

	float m_nearPlane = 1;
	float m_farPlane = 25;
};