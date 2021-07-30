#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"

class TextureTest : public Script
{
public:
	void OnStart() 
	{
		m_tile = std::make_shared<OpenGLTexture2D>("Resources/Textures/Tiles.PNG");
	}

	void OnRender()
	{
		Renderer3D::DrawQuad(m_quad.GetTransformMatrix(), m_tile, 10);
	}

private:
	std::shared_ptr<OpenGLCubeMap> m_cubeMap;
	std::shared_ptr<OpenGLTexture2D> m_tile;
	Transform m_c1;
	Transform m_c2 = Transform({2, 0, 0});
	Transform m_quad = Transform({ 0, 0, 0 }, { 0, 0, 0 }, {10, 10, 1});
};