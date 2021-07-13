#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>

enum class ShaderDataType
{
	None = 0,
	Float, Float2, Float3, Float4,
	Mat3, Mat4,
	Int, Int2, Int3, Int4,
	Bool
};

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

class BufferLayout
{
public:
	BufferLayout() { }

	BufferLayout(const std::initializer_list<BufferElement>& elementList) : m_elements(elementList)
	{
		CalculateOffsetsAndStride();
	}

	inline const unsigned int GetStride() const { return m_stride; }
	inline const std::vector<BufferElement>& GetElements() const { return m_elements; }

	std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_elements.end(); }

	std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

private:

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
	OpenGLVertexBuffer(unsigned int size);
	OpenGLVertexBuffer(float* vertices, unsigned int size);
	virtual ~OpenGLVertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(const void* data, unsigned int size);

	void SetLayout(const BufferLayout& l) { m_layout = l; }
	inline const BufferLayout& GetLayout() const { return m_layout; }

private:
	unsigned int m_rendererID;
	BufferLayout m_layout;
};

class OpenGLIndexBuffer
{
public:
	OpenGLIndexBuffer();
	OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
	virtual ~OpenGLIndexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(unsigned int* indices, unsigned int count);

	inline unsigned int GetCount() const { return m_count; }

private:
	unsigned int m_rendererID;
	unsigned int m_count;
};

