#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"

class TextureTest : public Script
{
public:
	void OnStart() 
	{
		m_tile = std::make_shared<OpenGLTexture2D>("Resources/Textures/Bricks.PNG");
		m_cubeMap = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");
		m_cubeMapTiles = std::make_shared<OpenGLCubeMap>("Resources/Textures/Tiles.PNG");
	}

	void OnRender()
	{
		Renderer3D::DrawVoxel(m_c1.GetTransformMatrix(), m_cubeMap);
		Renderer3D::DrawVoxel(m_c2.GetTransformMatrix(), m_cubeMapTiles);
		Renderer3D::DrawQuad(m_quad.GetTransformMatrix(), m_tile);
	}

private:
	std::shared_ptr<OpenGLCubeMap> m_cubeMap;
	std::shared_ptr<OpenGLCubeMap> m_cubeMapTiles;
	std::shared_ptr<OpenGLTexture2D> m_tile;
	Transform m_c1;
	Transform m_c2 = Transform({-2, 0, 0});
	Transform m_quad = Transform({ 2, 0, 0 });
};