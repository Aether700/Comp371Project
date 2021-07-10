#pragma once

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

