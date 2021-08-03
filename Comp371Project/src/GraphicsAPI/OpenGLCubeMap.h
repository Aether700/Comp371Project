#pragma once
#define GLEW_STATIC
#include "OpenGLTexture.h"
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"

#include <array>
#include <string>

//cube map used to generate textures for the voxels produced by the batch renderer
class OpenGLCubeMap : public OpenGLTexture
{
	friend class Renderer3D;
public:

	/*pass the texture used for each face where each texture 
	  is applied in this order

	  index 0 -> right face (positive X)
	  index 1 -> left face (negative X)
	  index 2 -> top face (positive Y)
	  index 3 -> bottom face (negative Y)
	  index 4 -> front face (positive Z) 
	  index 5 -> back face (negative Z)
	*/
	OpenGLCubeMap(const std::array<std::string, 6>& faceTextures);
	
	//will apply the same texture to every face
	OpenGLCubeMap(const std::string& faceTextures);

	//applies the texture data provided to all sides of the cube
	OpenGLCubeMap(unsigned int width, void* data);

	//creates an empty cubemap of the provided width and height with the specified internalDataFormat
	OpenGLCubeMap(unsigned int size, unsigned int internalDataFormat, unsigned int type = GL_UNSIGNED_BYTE);

	OpenGLCubeMap(unsigned int rendererID);

	~OpenGLCubeMap();

	virtual unsigned int GetRendererID() const { return m_rendererID; }

	//binds the cubemap to the provided texture slot (defaults to slot 0)
	virtual void Bind(unsigned int slot = 0) const override;

	unsigned int GetWidth() const override { return m_size; }
	unsigned int GetHeight() const override { return m_size; }

	virtual unsigned int GetTextureType() const override { return GL_TEXTURE_CUBE_MAP; }

	//check if this cubemap is equal to the provided texture
	virtual bool operator==(const OpenGLTexture& other) const override
	{
		const OpenGLCubeMap* ptr = dynamic_cast<const OpenGLCubeMap*>(&other);

		if (ptr == nullptr)
		{
			return false;
		}
		return ptr->m_rendererID == m_rendererID;
	}

private:

	unsigned int m_rendererID;

	//all textures need to have the same width and height
	unsigned int m_size;

	//format for each of the different faces, indices are matched with 
	//the order in which the faces are passed to the constructor
	std::array<unsigned int, 6> m_internalFormat, m_dataFormat;
};

