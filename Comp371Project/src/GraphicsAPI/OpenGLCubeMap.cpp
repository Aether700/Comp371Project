#define GLEW_STATIC
#include "OpenGLCubeMap.h"
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Dependencies/stb_image/stb_image.h"
#include "../Core/Debug.h"

#include <iostream>
#include <assert.h>
#include <math.h>


/*pass the texture used for each face where each texture
  is applied in this order

  index 0 -> right face (positive X)
  index 1 -> left face (negative X)
  index 2 -> top face (positive Y)
  index 3 -> bottom face (negative Y)
  index 4 -> front face (positive Z)
  index 5 -> back face (negative Z)
*/
OpenGLCubeMap::OpenGLCubeMap(const std::array<std::string, 6>& faceTextures) : m_size(0)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
	Bind();

	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data;
	
	for (size_t i = 0; i < 6; i++)
	{
		data = stbi_load(faceTextures[i].c_str(), &width, &height, &channels, 0);

		if (data == nullptr)
		{
			std::cout << "Failed to load image\n";
			assert(false);
		}
		
		unsigned int size = std::min(width, height);


		if ((m_size != 0) && (size != m_size))
		{
			std::cerr << "all textures passed to the cube map must have the same size\n";
		}
		else if (m_size == 0)
		{
			m_size = size;
		}

		switch (channels)
		{
		case 3:
			m_internalFormat[i] = GL_RGB8;
			m_dataFormat[i] = GL_RGB;
			break;

		case 4:
			m_internalFormat[i] = GL_RGBA8;
			m_dataFormat[i] = GL_RGBA;
			break;

		default:
			std::cerr << "The Format provided for the texture '" << faceTextures[i] << "' is not supported\n";
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size, 
			0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

//will apply the same texture to every face
OpenGLCubeMap::OpenGLCubeMap(const std::string& faceTexture)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
	Bind();
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	
	
	unsigned char* data = stbi_load(faceTexture.c_str(), &width, &height, &channels, 0);

	

	if (data == nullptr)
	{
		std::cout << "Failed to load image\n";
		assert(false);
	}
	
	m_size = std::min(width, height);
	
	switch (channels)
	{
	case 3:
		m_internalFormat[0] = GL_RGB8;
		m_dataFormat[0] = GL_RGB;
		break;

	case 4:
		m_internalFormat[0] = GL_RGBA8;
		m_dataFormat[0] = GL_RGBA;
		break;

	default:
		std::cout << "The Format provided for the texture '" << faceTexture << "' is not supported\n";
		assert(false);
	}

	for (size_t i = 0; i < 6; i++)
	{
		m_internalFormat[i] = m_internalFormat[0];
		m_dataFormat[i] = m_dataFormat[0];

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
			0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);
	}

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
	stbi_image_free(data);
}

//applies the texture data provided to all sides of the cube
OpenGLCubeMap::OpenGLCubeMap(unsigned int size, void* data) 
	: m_size(size)
{
	assert(data != nullptr && size > 0);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
	m_internalFormat[0] = GL_RGB8;
	m_dataFormat[0] = GL_RGB;

	Bind();

	for (size_t i = 0; i < 6; i++)
	{
		m_internalFormat[i] = m_internalFormat[0];
		m_dataFormat[i] = m_dataFormat[0];

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
			0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);
	}
	/* //to remove
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	*/

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

//creates an empty cubemap of the provided width and height with the specified internalDataFormat
OpenGLCubeMap::OpenGLCubeMap(unsigned int size, unsigned int internalDataFormat, unsigned int type) 
	: m_size(size)
{
	assert(size > 0);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
	m_internalFormat[0] = internalDataFormat;
	m_dataFormat[0] = internalDataFormat;

	Bind();

	for (size_t i = 0; i < 6; i++)
	{
		m_internalFormat[i] = m_internalFormat[0];
		m_dataFormat[i] = m_dataFormat[0];

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
			0, m_dataFormat[i], type, nullptr);
	}

	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

OpenGLCubeMap::OpenGLCubeMap(unsigned int rendererID) 
	: m_rendererID(rendererID), m_size(0) { }

OpenGLCubeMap::~OpenGLCubeMap()
{
	if (m_size != 0)
	{
		glDeleteTextures(1, &m_rendererID);
	}
}

//binds the cubemap to the provided texture slot (defaults to slot 0)
void OpenGLCubeMap::Bind(unsigned int slot) const
{
	glBindTextureUnit(slot, m_rendererID);
}
