#include "OpenGLBuffer.h"
#include <GL/glew.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//creates an opengl vertex buffer and reserves the provided size on the gpu for future data
OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int size)
{
	//glCreateBuffers(1, &m_rendererID);
    glGenBuffers(1,&m_rendererID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

//creates an opengl vertex buffer and passes the data provided to the gpu
OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, unsigned int size)
{
	//glCreateBuffers(1, &m_rendererID);
    glGenBuffers(1, &m_rendererID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

//deletes the vertex buffer object
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void OpenGLVertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//set the provided data in the vertex buffer
void OpenGLVertexBuffer::SetData(const void* data, unsigned int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Index Buffer //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//creates an opengl index buffer
OpenGLIndexBuffer::OpenGLIndexBuffer() : m_count(0)
{
	//glCreateBuffers(1, &m_rendererID);
    glGenBuffers(1, &m_rendererID);
    Bind();
}

//creates an opengl index buffer and passes the provided indices to it
OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int c) : m_count(c)
{
	//glCreateBuffers(1, &m_rendererID);
    glGenBuffers(1, &m_rendererID);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

void OpenGLIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void OpenGLIndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//sets the indices provided to the index buffer
void OpenGLIndexBuffer::SetData(unsigned int* indices, unsigned int count)
{
	m_count = count;
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
}
