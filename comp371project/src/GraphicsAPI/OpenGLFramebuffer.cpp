#include "OpenGLFramebuffer.h"
#include <GL/glew.h>

#include <assert.h>

OpenGLFramebuffer::OpenGLFramebuffer(unsigned int width, unsigned int height) 
	: m_rendererID(0)
{
	assert(!(width < 1 || height < 1));
	Resize(width, height);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &m_rendererID);
}

void OpenGLFramebuffer::Bind() const 
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
}

void OpenGLFramebuffer::Unbind() const 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int OpenGLFramebuffer::GetWidth() const 
{
	return m_width;
}

unsigned int OpenGLFramebuffer::GetHeight() const
{
	return m_height;
}

unsigned int OpenGLFramebuffer::GetColorAttachment() const
{
	return m_colorAttachment;
}

unsigned int OpenGLFramebuffer::GetDepthAttachment() const
{
	return m_depthAttachment;
}

void OpenGLFramebuffer::Resize(unsigned int width, unsigned int height)
{
	if (width < 1 || height < 1)
	{
		return;
	}

	//if there was a previous framebuffer delete it and it's attachments
	if (m_rendererID != 0)
	{
		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}

	//create framebuffer
	glCreateFramebuffers(1, &m_rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	
	//create the color attachements to that frame buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
	glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//specify the mipmap level of the texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

	//create depth attachement
	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);

	//specify the mipmap level of the texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	//unbind framebuffer to avoid rendering to it by accident
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_width = width;
	m_height = height;
}
