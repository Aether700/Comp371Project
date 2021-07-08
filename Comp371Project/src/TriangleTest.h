#pragma once
#include "Core/Script.h"
#include "Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Dependencies/glm-0.9.9.8/glm/mat3x3.hpp"

class TriangleTest : public Script
{
public:
	void OnStart()
	{
		//create vertex array
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glEnableVertexAttribArray(0);

		//create vertex buffer and pass it our data
		glCreateBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		
		float pos[] = {
			 0.0f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

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
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	unsigned int m_vao;
	unsigned int m_vbo;
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