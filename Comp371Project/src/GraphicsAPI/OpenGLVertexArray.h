#pragma once
#include "OpenGLBuffer.h"
#include <memory>

class OpenGLVertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	void Bind() const;
	void Unbind() const;

	void AddVertexBuffer(const std::shared_ptr<OpenGLVertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<OpenGLIndexBuffer>& indexBuffer);

	virtual const std::vector<std::shared_ptr<OpenGLVertexBuffer>>& GetVertexBuffers() const { return m_vertexBuffers; }
	virtual const std::shared_ptr<OpenGLIndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

private:
	unsigned int m_rendererID;
	std::vector<std::shared_ptr<OpenGLVertexBuffer>> m_vertexBuffers;
	std::shared_ptr<OpenGLIndexBuffer> m_indexBuffer;
};

