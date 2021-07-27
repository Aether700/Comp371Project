#pragma once
#define GLEW_STATIC
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Core/Script.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLFramebuffer.h"
#include "../GraphicsAPI/OpenGLCubeMap.h"

#include <vector>
#include <sstream>

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/multiple_lights.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/point_shadows_soft.cpp

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
		Application::SetBackgroundColor({ 1, 1, 1, 1 });
		PrepareCubeAndPlane();
			
		m_framebuffer = std::make_shared<OpenGLFramebuffer>(1024, 1024, true);
		m_framebuffer->Unbind();

		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShader.glsl");
		m_shadowMapShader = std::make_shared<OpenGLShader>("Resources/Shaders/ShadowMap.glsl");
		GenerateShadowMap(m_shadowMapShader, m_framebuffer, m_light.position);
		//SetPointLight(m_shader, m_light.position, {1, 1, 1, 1}, 0);

		m_shader->Bind();
		m_shader->SetFloat3("lightPos", m_light.position);
		m_shader->SetInt("shadows", 1);
		m_shader->SetFloat("far_plane", Application::GetCamera()->GetPerspectiveFarClip());
		m_shader->SetInt("depthMap", 0);
		//SetupDebugger();
	}

	void OnRender()
	{
		auto camera = Application::GetCamera();
		glm::mat4 camTransform = camera->GetTransform();
		glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camTransform;

		m_shader->Bind();
		m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		//m_shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);
		//m_shader->SetInt("u_shadowMap", m_framebuffer->GetDepthAttachment());
		m_shader->SetFloat3("viewPos", glm::vec3(camTransform[3][0], camTransform[3][1], camTransform[3][2]));

		m_framebuffer->BindDepthAttachment();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		/*
		//draw debugger quad
		m_shadowMapDebugger->Bind();
		m_shadowMapDebugger->SetInt("u_shadowMap", m_framebuffer->GetDepthAttachment());
		m_vaoDebugger->Bind();
		m_iboDebugger->Bind();
		glDrawElements(GL_TRIANGLES, m_iboDebugger->GetCount(), GL_UNSIGNED_INT, nullptr);
		*/
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
		LightVertexData cubePlanePos[2 * numVertices];

		unsigned int index = 0;
		//cube
		for (int i = 0; i < numVertices * 2; i+=2)
		{
			cubePlanePos[index].position = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 1);
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		//plane
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 1);
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		/*
		//light
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 1);
			cubePlanePos[index].mat = m_defaultMat;
			index++;
		}
		*/
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

		for (int i = 0; i < 2; i++)
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

	void GenerateShadowMap(std::shared_ptr<OpenGLShader>& shader,
		std::shared_ptr<OpenGLFramebuffer>& framebuffer, glm::vec3 position)
	{
		/*
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		// create depth cubemap texture
		unsigned int depthCubemap;
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
		m_cubemap = depthCubemap;

		float near_plane = 1.0f;
		float far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(m_light.position, m_light.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// 1. render scene to depth cubemap
		// --------------------------------
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_shadowMapShader->Bind();
		for (unsigned int i = 0; i < 6; ++i)
			m_shadowMapShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		m_shadowMapShader->SetFloat("far_plane", far_plane);
		m_shadowMapShader->SetFloat3("lightPos", m_light.position);
		//render scene
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		*/

		/*
		*/
		std::shared_ptr<OpenGLTexture> cubemap = std::make_shared<OpenGLCubeMap>(m_framebuffer->GetWidth(), 
			m_framebuffer->GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT);

		m_framebuffer->SetDepthAttachment(cubemap);


		//to avoid peter panning we cull the front face when generating the 
		//shadow map (works only for solid objects like cubes)
		glCullFace(GL_FRONT);

		//might need to change since currently calculating as a directional light
		//m_lightSpaceMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f)
		//	* glm::lookAt(position, m_cube.position, { 0, 1, 0 });

		cubemap->Bind();
		glm::mat4 lightProjMatrix = glm::perspective(glm::radians(90.0f), 
			(float)m_framebuffer->GetWidth() / (float)m_framebuffer->GetHeight(), 1.0f, 25.0f);

		std::vector<glm::mat4> lightTransforms;
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
			m_light.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); 
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
			m_light.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
			m_light.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
			m_light.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
			m_light.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		lightTransforms.push_back(lightProjMatrix * glm::lookAt(m_light.position, 
				m_light.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


		m_shadowMapShader->Bind();
		for (unsigned int i = 0; i < 6; i++)
		{
			m_shadowMapShader->SetMat4("u_shadowMatrices[" + std::to_string(i) + "]", lightTransforms[i]);
		}
		m_shadowMapShader->SetFloat("u_farPlane", Application::GetCamera()->GetPerspectiveFarClip());
		m_shadowMapShader->SetFloat3("u_lightPos", m_light.position);

		
		framebuffer->Bind();
		//store the current window width and height and change the 
		//viewport size to fit the whole shadow map texture
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());

		glClear(GL_DEPTH_BUFFER_BIT);

		//render the scene
		m_vao->Bind();
		m_ibo->Bind();
		cubemap->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		////////////////////

		//revert the cull face settings and the viewport size to their previous values for the actual rendering
		framebuffer->Unbind();
		glViewport(0, 0, windowWidth, windowHeight);
		glCullFace(GL_BACK);

		/*
		//to avoid peter panning we cull the front face when generating the 
		//shadow map (works only for solid objects like cubes)
		glCullFace(GL_FRONT);

		//might need to change since currently calculating as a directional light
		m_lightSpaceMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f)
			* glm::lookAt(position, m_cube.position, {0, 1, 0});


		shader->Bind();
		shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);

		framebuffer->Bind();
		//store the current window width and height and change the 
		//viewport size to fit the whole shadow map texture
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		//render the scene
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		////////////////////

		//revert the cull face settings and the viewport size to their previous values for the actual rendering
		framebuffer->Unbind();
		glViewport(0, 0, windowWidth, windowHeight);
		glCullFace(GL_BACK);
		*/
	}

	void SetupDebugger()
	{
		m_vaoDebugger = std::make_shared<OpenGLVertexArray>();

		float vertexData[] = {
			-0.25f, -0.25f, 0.0f, 0.0f,
			-0.5f, -0.25f, 1.0f, 0.0f,
			-0.25f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 1.0f, 1.0f,
		};

		m_vboDebugger = std::make_shared<OpenGLVertexBuffer>(vertexData, sizeof(vertexData));
		m_vboDebugger->SetLayout({ 
			{ShaderDataType::Float2, "pos"}, 
			{ShaderDataType::Float2, "textureCoord"}, 
		});

		m_vaoDebugger->AddVertexBuffer(m_vboDebugger);

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_iboDebugger = std::make_shared<OpenGLIndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));

		m_shadowMapDebugger = std::make_shared<OpenGLShader>("Resources/Shaders/ShadowMapDebugger.glsl");
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;
	std::shared_ptr<OpenGLShader> m_shadowMapShader;

	std::shared_ptr<OpenGLVertexArray> m_vaoDebugger;
	std::shared_ptr<OpenGLVertexBuffer> m_vboDebugger;
	std::shared_ptr<OpenGLIndexBuffer> m_iboDebugger;
	std::shared_ptr<OpenGLShader> m_shadowMapDebugger;

	std::shared_ptr<OpenGLFramebuffer> m_framebuffer;

	glm::mat4 m_lightSpaceMatrix;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	Transform m_light = Transform({ 0.5f, 1.5f, -2 }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});

	Material m_defaultMat = { {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 1}, 32 };
	Material m_testMat = { {0.1f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.75f, 0, 0}, 32 };

	//temp
	unsigned int m_fbo;
	unsigned int m_cubemap;
};