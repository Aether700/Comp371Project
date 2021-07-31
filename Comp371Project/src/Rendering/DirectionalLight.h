#pragma once
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../GraphicsAPI/OpenGLShader.h"

#include <memory>

class DirectionLight
{
public:
	DirectionLight(glm::vec4 color = {1, 1, 1, 1}) : m_color(color), 
		m_shader(std::make_shared<OpenGLShader>("../Resources/Shaders/DirectionalShadowMap.glsl")) { }


	void BindShadowMap()
	{

	}

private:
	glm::vec4 m_color;
	std::shared_ptr<OpenGLShader> m_shader;
};