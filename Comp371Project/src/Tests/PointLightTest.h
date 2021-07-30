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
		//GenerateShadowMap(m_shadowMapShader, m_framebuffer, m_light.position);
		GenerateShadowMapLab();
		//SetPointLight(m_shader, m_light.position, {1, 1, 1, 1}, 0);

		m_shader->Bind();
		m_shader->SetFloat3("lightPos", m_light.position);
		m_shader->SetInt("shadows", 0);
		m_shader->SetFloat("far_plane", Application::GetCamera()->GetPerspectiveFarClip());
		m_shader->SetInt("u_shadowMap", 0);
	}

	void OnUpdate()
	{
		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			m_light.position.z -= 5*Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			m_light.position.z += 5*Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_A))
		{
			m_light.position.x -= 5 * Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_D))
		{
			m_light.position.x += 5 * Time::GetDeltaTime();
		}

	}


	void OnRender()
	{
		auto camera = Application::GetCamera();
		glm::mat4 camTransform = camera->GetTransform();
		glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camTransform;

		m_shader->Bind();
		m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		m_shader->SetMat4("u_lightSpaceMatrix", glm::mat4(1.0f));
		m_shader->SetInt("u_shadowMap", 0);
		m_shader->SetFloat3("viewPos", Application::GetCameraController()->GetCamPos());
		

		//for debugging the shadow map
		//GenerateShadowMapLab();

		/*
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glm::mat4 lookAtMat = glm::lookAt(m_light.position, m_cube.position, { 0, 1, 0 });
		//glm::mat4 perspectiveMat = glm::perspective(50.0f, 1.0f, 0.01f, 400.0f);
		
		float orthographicSize = Application::GetCamera()->GetOrthographicSize();
		float aspectRatio = (float)windowWidth / (float)windowHeight;
		float orthoLeft = -orthographicSize * aspectRatio * 0.5f;
		float orthoRight = orthographicSize * aspectRatio * 0.5f;
		float orthoBottom = -orthographicSize * 0.5f;
		float orthoTop = orthographicSize * 0.5f;
		glm::mat4 perspectiveMat = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop);
		*/
		
		float lightNearPlane = 0.01f;
		float lightFarPlane = 400.0f;

		glm::mat4 lightProjMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
			//glm::perspective(50.0f, (float)1024 / (float)1024, lightNearPlane, lightFarPlane);
		glm::mat4 lightViewMatrix = glm::lookAt(m_light.position, m_cube.position, glm::vec3(0, 1, 0));
		
		m_lightSpaceMatrix = lightProjMatrix * lightViewMatrix;

		/*
		float near_plane = 0.1f;
		float far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		//glm::mat4 lightView = glm::lookAt(m_light.position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightView = glm::lookAt(m_light.position, m_cube.position, glm::vec3(0.0, 1.0, 0.0));
		m_lightSpaceMatrix = lightProjection * lightView;

		*/
		//m_shadowMapShader->Bind();
		//m_shadowMapShader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_shadowMapID);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		/*
		m_vaoLight->Bind();
		m_iboLight->Bind();
		glDrawElements(GL_TRIANGLES, m_iboLight->GetCount(), GL_UNSIGNED_INT, nullptr);
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
			cubePlanePos[index].mat = m_testMat;
			index++;
		}


		//plane
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		index = 0;

		//light
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			lightPosAndNormal[index].position = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			lightPosAndNormal[index].normal = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0);
			lightPosAndNormal[index].mat = m_defaultMat;
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

		m_vboLight = std::make_shared<OpenGLVertexBuffer>((float*)(void*)lightPosAndNormal, sizeof(lightPosAndNormal));
		m_vboLight->SetLayout({
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float3, "normal"},
			{ShaderDataType::Float3, "ambiant"},
			{ShaderDataType::Float3, "diffuse"},
			{ShaderDataType::Float3, "specular"},
			{ShaderDataType::Float, "shininess"}
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

	void GenerateShadowMap(std::shared_ptr<OpenGLShader>& shader,
		std::shared_ptr<OpenGLFramebuffer>& framebuffer, glm::vec3 position)
	{
		//to avoid peter panning we cull the front face when generating the 
		//shadow map (works only for solid objects like cubes)
		glCullFace(GL_FRONT);

		//might need to change since currently calculating as a directional light
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glm::mat4 lookAtMat = glm::lookAt(position, m_cube.position, { 0, 1, 0 });
		glm::mat4 perspectiveMat = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight,
			Application::GetCamera()->GetPerspectiveNearClip(), Application::GetCamera()->GetPerspectiveFarClip());
		m_lightSpaceMatrix = perspectiveMat * lookAtMat;

		std::cout << position.x << ", " << position.y << ", " << position.z << "\n\n";
		PrintMat4(lookAtMat);
		shader->Bind();
		shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);

		//store the current window width and height and change the 
		//viewport size to fit the whole shadow map texture
		glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		framebuffer->Bind();
		glBindTexture(GL_TEXTURE_2D, framebuffer->GetDepthAttachment());

		//render the scene
		shader->Bind();
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		////////////////////

		//revert the cull face settings and the viewport size to their previous values for the actual rendering
		framebuffer->Unbind();
		glViewport(0, 0, windowWidth, windowHeight);
		glCullFace(GL_BACK);
	}

	void GenerateShadowMapLearnOpengl()
	{
		// configure depth map FBO
		// -----------------------
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(m_light.position, m_cube.position, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		m_shadowMapShader->Bind();
		m_shadowMapShader->SetMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		int width, height;
		glfwGetFramebufferSize(Application::GetWindow(), &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GenerateShadowMapLab()
	{
		constexpr unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;

		// Variable storing index to texture used for shadow mapping
		GLuint depth_map_texture;
		// Get the texture
		glGenTextures(1, &depth_map_texture);
		// Bind the texture so the next glTex calls affect it
		glBindTexture(GL_TEXTURE_2D, depth_map_texture);
		// Create the texture and specify it's attributes, including widthn height,
		// components (only depth is stored, no color information)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE,
			DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		// Set texture sampler parameters.
		// The two calls below tell the texture sampler inside the shader how to
		// upsample and downsample the texture. Here we choose the nearest filtering
		// option, which means we just use the value of the closest pixel to the
		// chosen image coordinate.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// The two calls below tell the texture sampler inside the shader how it
		// should deal with texture coordinates outside of the [0, 1] range. Here we
		// decide to just tile the image.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Variable storing index to framebuffer used for shadow mapping
		GLuint depth_map_fbo;  // fbo: framebuffer object
		// Get the framebuffer
		glGenFramebuffers(1, &depth_map_fbo);
		// Bind the framebuffer so the next glFramebuffer calls affect it
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
		// Attach the depth map texture to the depth map framebuffer
		// glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		// depth_map_texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
			depth_map_texture, 0);
		//glDrawBuffer(GL_NONE);  // disable rendering colors, only write depth values
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_fbo = depth_map_fbo;
		m_shadowMapID = depth_map_texture;

		// Use proper shader
		m_shadowMapShader->Bind();
		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
		// Clear depth data on the framebuffer
		glClear(GL_DEPTH_BUFFER_BIT);

		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		int width, height;
		glfwGetFramebufferSize(Application::GetWindow(), &width, &height);
		glViewport(0, 0, width, height);
		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;
	std::shared_ptr<OpenGLShader> m_shadowMapShader;

	std::shared_ptr<OpenGLVertexArray> m_vaoLight;
	std::shared_ptr<OpenGLVertexBuffer> m_vboLight;
	std::shared_ptr<OpenGLIndexBuffer> m_iboLight;

	std::shared_ptr<OpenGLFramebuffer> m_framebuffer;

	glm::mat4 m_lightSpaceMatrix;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	Transform m_light = Transform({ 0, 0, 10 }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});

	Material m_defaultMat = { {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 1}, 32 };
	Material m_testMat = { {0.1f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.75f, 0, 0}, 32 };


	//temp
	unsigned int m_fbo;
	unsigned int m_shadowMapID;
	CameraController* m_camController;
};