#pragma once
#include "OpenGLTexture.h"

#include <array>
#include <string>

//temp: https://learnopengl.com/Advanced-OpenGL/Cubemaps

//cube map used to generate textures for the voxels produced by the batch renderer
class OpenGLCubeMap : public OpenGLTexture
{
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

	OpenGLCubeMap(unsigned int width, unsigned int height, void* data);

	~OpenGLCubeMap();

	virtual void Bind(unsigned int index = 0) const override;

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
	unsigned int m_width, m_height;

	//format for each of the different faces, indices are matched with 
	//the order in which the faces are passed to the constructor
	std::array<unsigned int, 6> m_internalFormat, m_dataFormat;
};

