#pragma once
#include <string>

class OpenGLTexture2D
{
public:
	OpenGLTexture2D(unsigned int width, unsigned int height);
	OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D();

	inline unsigned int GetRendererID() const { return m_rendererID; }

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	void SetData(void* data, unsigned int size);

	void Bind(unsigned int slot = 0) const;

	bool operator==(const OpenGLTexture2D& other) const
	{
		return m_rendererID == other.m_rendererID;
	}

	bool operator!=(const OpenGLTexture2D& other) const
	{
		return !(*this == other);
	}

private:
	std::string m_path;
	unsigned int m_width, m_height;
	unsigned int m_rendererID;
	unsigned int m_internalFormat;
	unsigned int m_dataFormat;
};

