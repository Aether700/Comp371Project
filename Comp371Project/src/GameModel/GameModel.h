#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"


//base class for every model
class GameModel : public Script
{
public:

	static void Init();

	virtual ~GameModel() { }

	std::shared_ptr<Transform>& GetTransform() { return m_transform; }
	std::shared_ptr<Transform>& GetModelTransform() { return m_modelTransform; }
	std::shared_ptr<Transform>& GetWallTransform() { return m_wallTransform; }

	void Select() { m_isSelected = true; }
	void Unselect() { m_isSelected = false; }

	//virtual float getScore() = 0;
	virtual float getScore() { return 0.0f;  }

protected:

	GameModel() : m_modelTransform(std::make_shared<Transform>()),
		m_wallTransform(std::make_shared<Transform>()), m_transform(std::make_shared<Transform>())
	{
		m_modelTransform->SetParent(m_transform);
		m_wallTransform->SetParent(m_transform);
	}

	GameModel(std::shared_ptr<Transform> worldTransform) : m_modelTransform(std::make_shared<Transform>()),
		m_wallTransform(std::make_shared<Transform>()), m_transform(std::make_shared<Transform>())
	{
		m_modelTransform->SetParent(m_transform);
		m_wallTransform->SetParent(m_transform);
		m_transform->SetParent(worldTransform);

	}

	void RenderCube(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		Renderer3D::DrawVoxel(transform, modelTexture, 1.0f, color);

		glm::mat4 WireTransform = transform;
		WireTransform[0][0] = WireTransform[0][0] * 1.05f;//x
		WireTransform[1][1] = WireTransform[1][1] * 1.05f;//y
		WireTransform[2][2] = WireTransform[2][2] * 1.05f;//z
		//Renderer3D::DrawWireCube(WireTransform, m_wireMaterial, glm::vec4(1 - color.x, 1 - color.y, 1 - color.z, color.w));// opposite color
		//Renderer3D::DrawWireCube(WireTransform, m_wireMaterial, glm::vec4(35.0f/255.0f, 20.0f/255.0f, 1, 1));
	}

	void RenderWall(const glm::mat4& transform, const glm::vec4& color = { 1, 0, 0, 1 })
	{
		Renderer3D::DrawVoxel(transform, wallTexture, 1.0f, {1,1,1,1});
		glm::mat4 WireTransform = transform;

		WireTransform[0][0] = WireTransform[0][0] * 1.0005f;//x
		WireTransform[1][1] = WireTransform[1][1] * 1.0005f;//y
		WireTransform[2][2] = WireTransform[2][2] * 1.0005f;//z
		//Renderer3D::DrawWireCube(WireTransform, m_wireMaterial, glm::vec4(1 - color.x, 1 - color.y, 1 - color.z, color.w));// opposite color
	}

	static std::shared_ptr<OpenGLCubeMap> wallTexture;
	static std::shared_ptr<OpenGLCubeMap> modelTexture;

	int wallcount = 0;
	bool m_isSelected = false;

private:

	std::shared_ptr<Transform> m_transform;
	std::shared_ptr<Transform> m_modelTransform;
	std::shared_ptr<Transform> m_wallTransform;
	

	Material m_wireMaterial = Material(true);
	Material m_modelMaterial = Material(true);// not in use for now

};