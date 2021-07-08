#include "OpenGLVertexArray.h"
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"

// temp function will change place later
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
			return GL_FLOAT;

		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return GL_INT;

		case ShaderDataType::Bool:
			return GL_BOOL;
	}

	std::cout << "Unknown Shader data type\n";
	assert(false);
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	glCreateVertexArrays(1, &m_rendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(m_rendererID);
}

void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<OpenGLVertexBuffer>& vertexBuffer)
{
	//make sure vertex buffer has a layout specified
	assert(vertexBuffer->GetLayout().GetElements().size());

	glBindVertexArray(m_rendererID);
	vertexBuffer->Bind();

	int index = 0;


	const BufferLayout& layout = vertexBuffer->GetLayout();


	for (const BufferElement& element : layout)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.type),
			element.normalized ? GL_TRUE : GL_FALSE,
			layout.GetStride(), (const void*)element.offset);
		index++;
	}
	m_vertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<OpenGLIndexBuffer>& index)
{
	glBindVertexArray(m_rendererID);
	index->Bind();
	m_indexBuffer = index;
}