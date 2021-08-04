#pragma once
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../Dependencies/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLFramebuffer.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "../Rendering/Renderer3D.h"

#include <memory>

class DirectionalLight
{
public:
	DirectionalLight() : m_shadowMapID(0), m_framebuffer(0) { }

	DirectionalLight(const glm::vec3& position, const glm::vec3& direction = { 0, 0, -1 },
		const glm::vec4& color = { 1, 1, 1, 1 }, unsigned int size = 512, bool drawLightCube = true)
		: m_color(color), m_position(position), m_direction(direction), m_size(size), m_drawLightCube(drawLightCube)
		//m_framebuffer(std::make_shared<OpenGLFramebuffer>(size, size)) 
	{
		if (s_shader == nullptr)
		{
			s_shader = std::make_shared<OpenGLShader>("Resources/Shaders/DirectionalShadowMap.glsl");
		}

		PrepareShadowMap();
	}

	~DirectionalLight()
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
		s_shader->SetMat4("u_lightSpaceMatrix", GetLightSpaceMatrix());

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
	
	std::shared_ptr<OpenGLTexture2D> GetShadowMap() const
	{
		return std::make_shared<OpenGLTexture2D>(m_shadowMapID, m_size, m_size);
	}

	glm::mat4 GetLightSpaceMatrix()
	{
		glm::mat4 lightProjection = glm::ortho(-m_orthogonalSize, m_orthogonalSize, 
			-m_orthogonalSize, m_orthogonalSize, m_nearPlane, m_farPlane);

		glm::vec3 up = { 0.0, 1.0, 0.0 };
		//glm::lookAt doesn't know what to do if the center direction and up direction are parallel
		//So we have to check if you want to look straight down or up (along the y axis)
		if (m_direction.x == 0 && m_direction.z == 0)
		{
			up = { 1.0, 0.0, 0.0 };
		}
		glm::mat4 lightView = glm::lookAt(m_position, m_position + m_direction, up);


		return lightProjection * lightView;
	}



	const glm::vec4& GetColor() const { return m_color; }

	const glm::vec3& GetPosition() const { return m_position; }

	float GetFarPlane() const { return m_farPlane; }
	float GetRadius() const { return m_radius; }

	DirectionalLight& operator=(DirectionalLight&& other) noexcept
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
		m_direction = other.m_direction;
		m_color = other.m_color;

		other.m_shadowMapID = 0;
		other.m_framebuffer = 0;

		return *this;
	}

	bool operator==(const DirectionalLight& other) const
	{
		return m_position == other.m_position && m_direction == other.m_direction;
	}

	bool operator!=(const DirectionalLight& other) const
	{
		return !(*this == other);
	}

private:
	static std::shared_ptr<OpenGLShader> s_shader;
	static Material s_glowMat;

	void PrepareShadowMap()
	{
		glGenFramebuffers(1, &m_framebuffer);
		// create depth texture
		glGenTextures(1, &m_shadowMapID);
		glBindTexture(GL_TEXTURE_2D, m_shadowMapID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//specify the color used for border clamping
		float clamp_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float m_radius = 100.0f;
	float m_orthogonalSize = 10.0f;
	float m_nearPlane = 1.0f;
	float m_farPlane = 50.0f;
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec4 m_color;
	bool m_drawLightCube;
	
	unsigned int m_framebuffer;
	unsigned int m_shadowMapID;
	unsigned int m_size;
};