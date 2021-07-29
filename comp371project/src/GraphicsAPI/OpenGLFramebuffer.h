#ifndef _OPENGL_FRAMEBUFFER_H_
#define _OPENGL_FRAMEBUFFER_H_

class OpenGLFramebuffer
{
public:
	OpenGLFramebuffer(unsigned int width, unsigned int height);
	~OpenGLFramebuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	unsigned int GetColorAttachment() const;
	unsigned int GetDepthAttachment() const;
	void Resize(unsigned int width, unsigned int height);

private:
	unsigned int m_rendererID;
	unsigned int m_width, m_height;

	unsigned int m_colorAttachment, m_depthAttachment;
};

#endif
