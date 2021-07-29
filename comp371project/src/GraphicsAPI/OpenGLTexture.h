#ifndef _OPENGL_TEXTURE_H_
#define _OPENGL_TEXTURE_H_

/*generic texture class which defines an interface which all textures 
  should implement (note that this class is not an actual c++ interface 
  as it defines a function)
*/
class OpenGLTexture
{
public:
	virtual ~OpenGLTexture() { }
	
	virtual void Bind(unsigned int slot = 0) const = 0;

	virtual bool operator==(const OpenGLTexture& other) const = 0;

	bool operator!=(const OpenGLTexture& other) const
	{
		return !(*this == other);
	}
};

#endif
