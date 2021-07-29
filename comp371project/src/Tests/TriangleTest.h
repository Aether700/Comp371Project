#ifndef _TRIANGLE_TEST_H_
#define _TRIANGLE_TEST_H_

#include "../Core/Script.h"
#include <GL/glew.h>
#include <glm/mat3x3.hpp>
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"

class TriangleTest : public Script
{
public:
	void OnStart()
	{
		//create vertex array
		m_vao = std::make_shared<OpenGLVertexArray>();


		float pos[] = {
			 0.0f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f
		};

		m_vbo = std::make_shared<OpenGLVertexBuffer>(pos, sizeof(pos));
		m_vbo->SetLayout({ { ShaderDataType::Float3, "position" } });

		m_vao->AddVertexBuffer(m_vbo);

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &m_vertexShaderSrc, nullptr);
		glCompileShader(vertexShader);

		unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShader, 1, &m_fragmentShaderSrc, nullptr);
		glCompileShader(fragShader);

		m_shaderProgram = glCreateProgram();
		glAttachShader(m_shaderProgram, vertexShader);
		glAttachShader(m_shaderProgram, fragShader);
		glLinkProgram(m_shaderProgram);
	}

	void OnRender()
	{
		//bind vertex array and vertex buffer before drawing since 
		//other stuff might have been bound before this is called
		glUseProgram(m_shaderProgram);
		m_vao->Bind();
		m_vbo->Bind();

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	unsigned int m_shaderProgram;
	const char* m_vertexShaderSrc =
		"#version 400\n"
		"in vec3 pos;"
		"void main()"
		"{"
		"gl_Position = vec4(pos, 1);"
		"}";

	const char* m_fragmentShaderSrc =
		"#version 400\n"
		"out vec4 outColor;"
		"void main()"
		"{"
		"outColor = vec4(0.8f, 0f, 0.5f, 1f);"
		"}";
};

#endif
