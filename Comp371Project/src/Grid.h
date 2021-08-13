#pragma once
#include "Core/Script.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Rendering/Renderer3D.h"


class Grid : public Script
{
public:

	Grid(std::shared_ptr<Transform> worldTransform)
	{
		m_texture = std::make_shared<OpenGLTexture2D>("Resources/Textures/sand.jpg");
		m_transformMesh = std::make_shared<Transform>(glm::vec3( 0, 0, 0 ), glm::vec3( 0, 0, 0 ), glm::vec3(1, 1, 1 ));
		m_transformQuad = std::make_shared<Transform>(glm::vec3(0, -0.5f, 0 ), glm::vec3(-glm::radians(90.0f), 0, 0 ), glm::vec3(100, 100, 1 ));
		m_transformQuad->SetParent(worldTransform);
		m_transformMesh->SetParent(worldTransform);
	}

	void OnRender()
	{
		if (isTextureOn)
		{
			Renderer3D::DrawQuad(m_transformQuad->GetTransformMatrix(), m_texture, tileFactor);
		}
		else
		{
			// started at -50 since it need to be center at origin assuming(0,0) being origin
			for (int displacement = -50; displacement <= 50; displacement++) 
			{
				Renderer3D::DrawLine(m_transformMesh->GetTransformMatrix(), { displacement, -0.5f, -50 }, { displacement, -0.5f, 50 });
				Renderer3D::DrawLine(m_transformMesh->GetTransformMatrix(), { -50, -0.5f, displacement }, { 50, -0.5f, displacement });
			}
		}
	}
	
	void ToggleTexture()
	{
		isTextureOn = !isTextureOn;
	}

	std::shared_ptr<Transform> GetTransformMesh() { return m_transformMesh; }
	std::shared_ptr<Transform> GetTransformQuad() { return m_transformQuad; }

private:


		std::shared_ptr<OpenGLTexture2D> m_texture;
		float tileFactor = 100.0f;

		std::shared_ptr<Transform> m_transformMesh;
		std::shared_ptr<Transform> m_transformQuad;
		
		bool isTextureOn = false;

};
