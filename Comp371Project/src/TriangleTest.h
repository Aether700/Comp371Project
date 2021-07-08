#pragma once
#include "Core/Script.h"
#include "Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Dependencies/glm-0.9.9.8/glm/mat3x3.hpp"

class TriangleTest : public Script
{
public:
	void OnAwake()
	{
		//create vertex array
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glEnableVertexAttribArray(0);

		//create vertex buffer and pass it our data
		glCreateBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		{
			float pos[] = {
				 0.0f,  0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				-0.5f, -0.5f, 0.0f,
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	void OnRender()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//bind vertex array and vertex buffer before drawing since 
		//other stuff might have been bound before this is called
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	unsigned int m_vao;
	unsigned int m_vbo;
};