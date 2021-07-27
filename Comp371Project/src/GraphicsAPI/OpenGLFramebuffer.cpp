#include "OpenGLFramebuffer.h"
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"

#include <assert.h>

OpenGLFramebuffer::OpenGLFramebuffer(unsigned int width, unsigned int height, bool useDepthOnly) 
	: m_rendererID(0), m_useDepthOnly(useDepthOnly)
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

void OpenGLFramebuffer::BindDepthAttachment(unsigned int slot) const
{
	glBindTextureUnit(slot, m_depthAttachment);
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

		if (!m_useDepthOnly && m_colorAttachmentTexture == nullptr)
		{
			glDeleteTextures(1, &m_colorAttachment);
		}

		if (m_depthAttachmentTexture == nullptr)
		{
			glDeleteTextures(1, &m_depthAttachment);
		}
	}

	//create framebuffer
	glCreateFramebuffers(1, &m_rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	
	if (!m_useDepthOnly)
	{
		//create the color attachements to that frame buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,
			height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//attach the texture to the framebuffer and specify the mipmap level of the texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);
	}


	//create depth attachement
	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);

	//attach the texture to the framebuffer and specify the mipmap level of the texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

	if (m_useDepthOnly)
	{
		//tell opengl that we won't be using the color attachment
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	//unbind framebuffer to avoid rendering to it by accident
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_width = width;
	m_height = height;
}

void OpenGLFramebuffer::SetColorAttachment(std::shared_ptr<OpenGLTexture>& texture)
{
	assert(texture->GetWidth() == m_width && texture->GetHeight() == m_height);

	if (m_colorAttachmentTexture == nullptr && m_colorAttachment != 0)
	{
		glDeleteTextures(1, &m_colorAttachment);
	}

	Bind();
	m_colorAttachmentTexture = texture;
	m_colorAttachment = texture->GetRendererID();
	
	//attach the texture to the framebuffer and specify the mipmap level of the texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorAttachment, 0);
	
	//unbind so we don't draw to the framebuffer accidentally
	Unbind();
}

void OpenGLFramebuffer::SetDepthAttachment(std::shared_ptr<OpenGLTexture>& texture)
{
	assert(texture->GetWidth() == m_width && texture->GetHeight() == m_height);

	if (m_depthAttachmentTexture == nullptr && m_depthAttachment != 0)
	{
		glDeleteTextures(1, &m_depthAttachment);
	}

	Bind();
	m_depthAttachmentTexture = texture;
	m_depthAttachment = texture->GetRendererID();

	//attach the texture to the framebuffer and specify the mipmap level of the texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthAttachment, 0);
	
	//unbind so we don't draw to the framebuffer accidentally
	Unbind();
}