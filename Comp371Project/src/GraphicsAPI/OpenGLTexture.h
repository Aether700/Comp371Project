#pragma once

/*generic texture class which defines an interface which all textures 
  should implement (note that this class is not an actual c++ interface 
  as it defines a function)
*/
class OpenGLTexture
{
public:
	virtual ~OpenGLTexture() { }
	
	virtual void Bind(unsigned int slot = 0) const = 0;

	virtual unsigned int GetRendererID() const = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	//returns the GLenum representing the texture type
	virtual unsigned int GetTextureType() const = 0;

	virtual bool operator==(const OpenGLTexture& other) const = 0;

	bool operator!=(const OpenGLTexture& other) const
	{
		return !(*this == other);
	}
};

