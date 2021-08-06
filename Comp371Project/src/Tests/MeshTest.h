#pragma once
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Rendering/Renderer3D.h"
#include "../Rendering/Mesh.h"
#include "../Core/Script.h"

class MeshTest : public Script
{
public:
	void OnStart()
	{
		m_mesh = Mesh::LoadFromFile("Resources/Models/cube.obj");

		if (m_mesh == nullptr)
		{
			assert(false);
		}
	}

	void OnRender()
	{
		Renderer3D::DrawMesh(glm::mat4(1.0f), m_mesh, Renderer3D::GetDefaultWhiteTexture());
	}

private:
	std::shared_ptr<Mesh> m_mesh;
};

