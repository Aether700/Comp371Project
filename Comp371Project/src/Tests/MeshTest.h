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
		//m_mesh = Mesh::LoadFromFile("Resources/Models/Lowest_poly_thinker.obj");
		m_mesh = Mesh::LoadFromFile("Resources/Models/Lowpoly_Ionic_Column.obj");
		//m_mesh = Mesh::LoadFromFile("Resources/Models/cube.obj");

		if (m_mesh == nullptr)
		{
			assert(false);
		}
		Renderer3D::UseShadows(true);
	}

	void OnUpdate()
	{
		m_meshTransform.rotation.z += m_rotSpeed * Time::GetDeltaTime();
		Renderer3D::UpdateLights();
	}

	void OnRender()
	{
		Renderer3D::DrawMesh(m_meshTransform.GetTransformMatrix(), m_mesh, Renderer3D::GetDefaultWhiteTexture(), 1, {1, 1, 1, 1});
		Renderer3D::DrawVoxel(m_cube.GetTransformMatrix(), glm::vec4{1, 0, 0, 1});
		Renderer3D::AddPointLight(m_meshTransform.position + glm::vec3{0, 15, 20});
		//glm::vec3 lightPos = m_meshTransform.position + glm::vec3{ 0, 15, 20 };
		//Renderer3D::AddDirectionalLight(lightPos, m_meshTransform.position - lightPos);
	}

private:
	std::shared_ptr<Mesh> m_mesh;
	//Transform m_meshTransform = Transform({ 0, 0, -10 }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});
	Transform m_meshTransform = Transform({ 0, 0, -10 }, { -glm::radians(90.0f), 0, 0 }, {0.1f, 0.1f, 0.1f});
	Transform m_cube = Transform({ 0, -10, -10 }, { 0, 0, 0 }, { 30, 0.5f, 30 });
	float m_rotSpeed = 0.3f;
};

