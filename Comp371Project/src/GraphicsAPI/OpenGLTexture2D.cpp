#define GLEW_STATIC
#include "OpenGLTexture2D.h"
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Dependencies/stb_image/stb_image.h"

#include <iostream>
#include <assert.h>


OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height)
	: m_internalFormat(GL_RGBA8), m_dataFormat(GL_RGBA), m_width(width), m_height(height)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
	glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height, void* data)
	: m_internalFormat(GL_RGBA8), m_dataFormat(GL_RGBA), m_width(width), m_height(height)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
	glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (data == nullptr)
	{
		std::cout << "Failed to load image\n";
		assert(false);
	}
	m_width = width;
	m_height = height;

	switch (channels)
	{
	case 3:
		m_internalFormat = GL_RGB8;
		m_dataFormat = GL_RGB;
		break;

	case 4:
		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;
		break;

	default:
		std::cerr << "The Format provided for the texture '" << path << "' is not supported\n";
	}

	if (m_internalFormat == 0)
	{
		std::cout << "Format not supported\n";
		assert(false);
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
	glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_rendererID);
}

void OpenGLTexture2D::SetData(void* data, unsigned int size)
{
	unsigned int bytesPerChannel = m_dataFormat == GL_RGBA ? 4 : 3;
	if (size != m_width * m_height * bytesPerChannel)
	{
		std::cout << "Data must be entire texture\n";
		assert(false);
	}

	glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::Bind(unsigned int slot) const
{
	glBindTextureUnit(slot, m_rendererID);
}