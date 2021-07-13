#pragma once
#include "OpenGLTexture.h"

#include <string>

class OpenGLTexture2D : public OpenGLTexture
{
public:
	OpenGLTexture2D(unsigned int width, unsigned int height);
	OpenGLTexture2D(unsigned int width, unsigned int height, void* data);
	OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D();

	inline unsigned int GetRendererID() const { return m_rendererID; }

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	void SetData(void* data, unsigned int size);

	virtual void Bind(unsigned int slot = 0) const override;

	virtual bool operator==(const OpenGLTexture& other) const override
	{
		const OpenGLTexture2D* ptr = dynamic_cast<const OpenGLTexture2D*>(&other);

		if (ptr == nullptr)
		{
			return false;
		}
		return ptr->m_rendererID == m_rendererID;
	}

private:
	std::string m_path;
	unsigned int m_width, m_height;
	unsigned int m_rendererID;
	unsigned int m_internalFormat;
	unsigned int m_dataFormat;
};

