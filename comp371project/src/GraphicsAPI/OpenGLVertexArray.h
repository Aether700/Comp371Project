#ifndef _OPENGL_VERTEX_ARRAY_H_
#define _OPENGL_VERTEX_ARRAY_H_
#include "OpenGLBuffer.h"
#include <memory>

//class which represent a vertex array in opengl
class OpenGLVertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	void Bind() const;
	void Unbind() const;

	/*adds a vertex buffer to this vertex array and will also specify the 
	  layout of the provided vertex buffer to the gpu by using glVertexAttribPointer
	*/
	void AddVertexBuffer(const std::shared_ptr<OpenGLVertexBuffer>& vertexBuffer);
	
	//sets the index buffer of this vertex array to the provided index buffer
	void SetIndexBuffer(const std::shared_ptr<OpenGLIndexBuffer>& indexBuffer);

	//retrieves the vertex buffers and index buffer of this vertex array
	virtual const std::vector<std::shared_ptr<OpenGLVertexBuffer>>& GetVertexBuffers() const { return m_vertexBuffers; }
	virtual const std::shared_ptr<OpenGLIndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

private:
	unsigned int m_rendererID;
	std::vector<std::shared_ptr<OpenGLVertexBuffer>> m_vertexBuffers;
	std::shared_ptr<OpenGLIndexBuffer> m_indexBuffer;
};

#endif

