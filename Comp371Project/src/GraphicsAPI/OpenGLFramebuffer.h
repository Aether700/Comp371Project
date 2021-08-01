#pragma once
#include "OpenGLTexture.h"

#include <memory>

class OpenGLFramebuffer
{
public:
	OpenGLFramebuffer(unsigned int width, unsigned int height);
	~OpenGLFramebuffer();

	void Bind() const;
	void Unbind() const;

	void BindDepthAttachment(unsigned int slot = 0) const;


	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	unsigned int GetColorAttachment() const;
	unsigned int GetDepthAttachment() const;
	void Resize(unsigned int width, unsigned int height);

	void SetColorAttachment(std::shared_ptr<OpenGLTexture>& texture);
	void SetDepthAttachment(std::shared_ptr<OpenGLTexture>& texture);

private:
	unsigned int m_rendererID;
	unsigned int m_width, m_height;

	unsigned int m_colorAttachment, m_depthAttachment;

	//store references to textures that were not created by the framebuffer 
	//so they don't get deleted while the framebuffer is still in use
	std::shared_ptr<OpenGLTexture> m_colorAttachmentTexture, m_depthAttachmentTexture;
};
