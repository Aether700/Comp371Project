#pragma once
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../Dependencies/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLFramebuffer.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "../Rendering/Renderer3D.h"

#include <memory>

class PointLight
{
public:
	PointLight() : m_shadowMapID(0), m_framebuffer(0) { }

	PointLight(const glm::vec3& position, const glm::vec4& color = { 1, 1, 1, 1 }, unsigned int size = 2048, 
		bool drawLightCube = true) : m_color(color), m_position(position), 
		m_size(size), m_drawLightCube(drawLightCube)
	{
		if (s_shader == nullptr)
		{
			s_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShadowMap.glsl");
		}

		PrepareShadowMap();
	}

	~PointLight()
	{
		if (m_shadowMapID != 0)
		{
			glDeleteTextures(1, &m_shadowMapID);
		}

		if (m_framebuffer != 0)
		{
			glDeleteFramebuffers(1, &m_framebuffer);
		}
	}

	void PrepareForShadowMapGeneration()
	{
		s_shader->Bind();
		SetLightMatricesUniforms();

		glViewport(0, 0, m_size, m_size);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
	}

	void DrawLightCube()
	{
		if (m_drawLightCube)
		{
			Renderer3D::DrawVoxel(m_position, { 0, 0, 0 }, { 2.0f, 2.0f, 2.0f }, s_glowMat);
		}
	}

	std::shared_ptr<OpenGLCubeMap> GetShadowMap() const
	{
		return std::make_shared<OpenGLCubeMap>(m_shadowMapID, m_size, m_size);
	}

	std::vector<glm::mat4> GetLightSpaceMatrices()
	{
		glm::mat4 lightProj = glm::perspective(glm::radians(90.0f),
			(float)m_size / (float)m_size, m_nearPlane, m_farPlane);

		std::vector<glm::mat4> lightSpacesMatrices;
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)));
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)));
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)));
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -1.0f)));
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)));
		lightSpacesMatrices.push_back(lightProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)));

		return lightSpacesMatrices;
	}

	const glm::vec4& GetColor() const { return m_color; }

	const glm::vec3& GetPosition() const { return m_position; }

	float GetFarPlane() const { return m_farPlane; }

	PointLight& operator=(PointLight&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}

		if (m_shadowMapID != 0)
		{
			glDeleteTextures(1, &m_shadowMapID);
		}

		if (m_framebuffer != 0)
		{
			glDeleteFramebuffers(1, &m_framebuffer);
		}

		m_framebuffer = other.m_framebuffer;
		m_shadowMapID = other.m_shadowMapID;
		m_size = other.m_size;
		m_nearPlane = other.m_nearPlane;
		m_farPlane = other.m_farPlane;
		m_position = other.m_position;
		m_color = other.m_color;

		other.m_shadowMapID = 0;
		other.m_framebuffer = 0;

		return *this;
	}

private:
	static std::shared_ptr<OpenGLShader> s_shader;
	static Material s_glowMat;

	void PrepareShadowMap()
	{
		glGenFramebuffers(1, &m_framebuffer);
		// create depth cubemap texture
		glGenTextures(1, &m_shadowMapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMapID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetLightMatricesUniforms()
	{
		auto lightMatrices = GetLightSpaceMatrices();

		for (unsigned int i = 0; i < 6; i++)
		{
			s_shader->SetMat4("u_lightSpaceMatrices[" + std::to_string(i) + "]", lightMatrices[i]);
		}
	}

	float m_nearPlane = 1.0f;
	float m_farPlane = 25.0f;
	glm::vec3 m_position;
	glm::vec4 m_color;
	bool m_drawLightCube;

	unsigned int m_framebuffer;
	unsigned int m_shadowMapID;
	unsigned int m_size;
};