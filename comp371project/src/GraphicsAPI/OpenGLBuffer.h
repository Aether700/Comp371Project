#ifndef _OPENGL_BUFFER_H_
#define _OPENGL_BUFFER_H_

#include <vector>
#include <string>
#include <assert.h>
#include <iostream>

//enum class which represents the data types that can be passed to shaders
enum class ShaderDataType
{
	None = 0,
	Float, Float2, Float3, Float4,
	Mat3, Mat4,
	Int, Int2, Int3, Int4,
	Bool
};

//helper function which returns the size in bytes for the provided ShaderDataType
static unsigned int ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:		return sizeof(float);
	case ShaderDataType::Float2:	return 2 * sizeof(float);
	case ShaderDataType::Float3:	return 3 * sizeof(float);
	case ShaderDataType::Float4:	return 4 * sizeof(float);
	case ShaderDataType::Mat3:		return 3 * 3 * sizeof(float); // 3x3 matrix
	case ShaderDataType::Mat4:		return 4 * 4 * sizeof(float); //4x4 matrix
	case ShaderDataType::Int:		return sizeof(int);
	case ShaderDataType::Int2:		return 2 * sizeof(int);
	case ShaderDataType::Int3:		return 3 * sizeof(int);
	case ShaderDataType::Int4:		return 4 * sizeof(int);
	case ShaderDataType::Bool:		return sizeof(bool);
	}

	std::cout << "unknown type\n";
	assert(false);
	return 0;
}

//struct which represents an element in the buffer layer of a vertex buffer 
struct BufferElement
{
	std::string name;
	ShaderDataType type;
	unsigned int size;
	unsigned int offset;
	bool normalized;

	BufferElement() { }

	BufferElement(ShaderDataType t, const std::string& n, bool nor = false) : name(n), type(t), offset(0),
		size(ShaderDataTypeSize(t)), normalized(nor) { }

	//returns the component count of the ShaderDataType stored in this buffer element
	unsigned int GetComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 1;
		case ShaderDataType::Float2:	return 2;
		case ShaderDataType::Float3:	return 3;
		case ShaderDataType::Float4:	return 4;
		case ShaderDataType::Mat3:		return 3 * 3; // 3x3 matrix
		case ShaderDataType::Mat4:		return 4 * 4; //4x4 matrix
		case ShaderDataType::Int:		return 1;
		case ShaderDataType::Int2:		return 2;
		case ShaderDataType::Int3:		return 3;
		case ShaderDataType::Int4:		return 4;
		case ShaderDataType::Bool:		return 1;
		}

		std::cout << "Unknown Shader data type\n";
		assert(false);
		return 0;
	}
};

//represents a buffer layout to be contained by the vertex buffer
class BufferLayout
{
public:
	BufferLayout() { }

	BufferLayout(const std::initializer_list<BufferElement>& elementList) : m_elements(elementList)
	{
		CalculateOffsetsAndStride();
	}

	//retrieve the strive of the layout
	inline const unsigned int GetStride() const { return m_stride; }

	//retrieve the list of buffer elements in the layout
	inline const std::vector<BufferElement>& GetElements() const { return m_elements; }


	//iterator functions for the buffer layout
	std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_elements.end(); }

	std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

private:

	//helper function which calculates the offset and stride of the layout
	void CalculateOffsetsAndStride()
	{
		unsigned int offset = 0;
		m_stride = 0;
		for (auto& element : m_elements)
		{
			element.offset = offset;
			offset += element.size;
			m_stride += element.size;
		}
	}

	std::vector<BufferElement> m_elements;
	unsigned int m_stride; //size of a Vertex
};

class OpenGLVertexBuffer
{
public:
	//creates an opengl vertex buffer and reserves the provided size on the gpu for future data
	OpenGLVertexBuffer(unsigned int size);

	//creates an opengl vertex buffer and passes the data provided to the gpu
	OpenGLVertexBuffer(float* vertices, unsigned int size);
	
	//deletes the vertex buffer object
	virtual ~OpenGLVertexBuffer();

	void Bind() const;
	void Unbind() const;

	//set the provided data in the vertex buffer
	void SetData(const void* data, unsigned int size);

	//sets the layout of the vertex buffer
	void SetLayout(const BufferLayout& l) { m_layout = l; }

	//retrives the buffer layout of this vertex buffer
	inline const BufferLayout& GetLayout() const { return m_layout; }

private:
	unsigned int m_rendererID;
	BufferLayout m_layout;
};

class OpenGLIndexBuffer
{
public:
	//creates an opengl index buffer
	OpenGLIndexBuffer();

	//creates an opengl index buffer and passes the provided indices to it
	OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
	virtual ~OpenGLIndexBuffer();

	void Bind() const;
	void Unbind() const;

	//sets the indices provided to the index buffer
	void SetData(unsigned int* indices, unsigned int count);

	//retrieves the number of indices contained in the index buffer
	inline unsigned int GetCount() const { return m_count; }

private:
	unsigned int m_rendererID;
	unsigned int m_count;
};

#endif
