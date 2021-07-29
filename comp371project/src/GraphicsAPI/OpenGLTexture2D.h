#ifndef _OPENGL_TEXTURE_2D_H_
#define _OPENGL_TEXTURE_2D_H_

#include "OpenGLTexture.h"

#include <string>

//class which represent a 2D texture in opengl
class OpenGLTexture2D : public OpenGLTexture
{
public:
	//defines an opengl 2D texture of the width and height provided without setting any texture data
	OpenGLTexture2D(unsigned int width, unsigned int height);

	//defines an opengl 2D texture of the width and height and texture data provided 
	OpenGLTexture2D(unsigned int width, unsigned int height, void* data);

	//reads the file at the file path provided and tries to create a 2D texture from it
	OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D();

	inline unsigned int GetRendererID() const { return m_rendererID; }

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	//sets the texture data of this texture object
	void SetData(void* data, unsigned int size);

	virtual void Bind(unsigned int slot = 0) const override;

	//checks to see if the other texture object is a 2D texture 
	//and if it maps to the same 2D texture as this object
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

#endif
