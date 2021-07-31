#pragma once
#include "Core/Script.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Rendering/Renderer3D.h"


class Grid : public Script
{
public:

	Grid()
	{
		m_texture = std::make_shared<OpenGLTexture2D>("Resources/Textures/Tiles.PNG");
	}

	void OnRender()
	{
		
		for (int displacement = -50; displacement <= 50; displacement++) // started at -50 since it need to be center at origin assuming(0,0) being origin
		{
			Renderer3D::DrawLine(m_transform1.GetTransformMatrix(), { displacement, 0, -50 }, { displacement, 0, 50 });
			Renderer3D::DrawLine(m_transform1.GetTransformMatrix(), { -50, 0, displacement }, { 50, 0, displacement });
		}

		if(isTextureOn)
		{
			Renderer3D::DrawQuad(m_transform.GetTransformMatrix(), m_texture, tileFactor);
		}
	}
	
	void ToggleTexture()
	{
		isTextureOn = !isTextureOn;
	}

private:


		std::shared_ptr<OpenGLTexture2D> m_texture;
		float tileFactor = 100.0f;
		Transform m_transform1 = Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Transform m_transform = Transform({ 0, -0.01, 0 }, { glm::radians(90.0f), 0, 0 }, { 100, 100, 100 });
		
		bool isTextureOn = false;

};
