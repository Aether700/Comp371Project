#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"
#include "../Core/Random.h"


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

	virtual float getScore() { return 0.0f;  }

protected:

	GameModel() : m_modelTransform(std::make_shared<Transform>()),
		m_wallTransform(std::make_shared<Transform>()), m_transform(std::make_shared<Transform>())
	{
		m_modelTransform->SetParent(m_transform);
		m_wallTransform->SetParent(m_transform);
	}

	
	void RenderCube(const glm::mat4& transform, const glm::vec4& color = glm::vec4(110.0f / 255.0f, 255.0f / 255.0f, 1, 1))
	{
		glm::mat4 ModelTransform = transform;
		ModelTransform[0][0] = ModelTransform[0][0] * 0.98f;//x
		ModelTransform[1][1] = ModelTransform[1][1] * 0.98f;//y
		ModelTransform[2][2] = ModelTransform[2][2] * 0.98f;//z
		Renderer3D::DrawVoxel(ModelTransform, modelTexture, 1.0f, color);

		glm::mat4 WireTransform = transform;
		WireTransform[0][0] = WireTransform[0][0] * 1.008f;//x
		WireTransform[1][1] = WireTransform[1][1] * 1.008f;//y
		WireTransform[2][2] = WireTransform[2][2] * 1.008f;//z
		Renderer3D::DrawWireCube(WireTransform, m_wireMaterial, { 1,1,1,1 });
	}

	void RenderWall(const glm::mat4& transform, const glm::vec4& color = { 1, 0, 0, 1 })
	{
		glm::mat4 ModelTransform = transform;
		ModelTransform[0][0] = ModelTransform[0][0] * 0.98f;//x
		ModelTransform[1][1] = ModelTransform[1][1] * 0.98f;//y
		ModelTransform[2][2] = ModelTransform[2][2] * 0.98f;//z
		Renderer3D::DrawVoxel(ModelTransform, modelTexture, 1.0f, {1,1,1,1});

		glm::mat4 WireTransform = transform;
		glm::vec3 position = glm::vec3(WireTransform[3][0], WireTransform[3][1], WireTransform[3][2]);

		Renderer3D::DrawWireCube(position, { 0, 0, 0 }, { m_wireScale, m_wireScale, m_wireScale}, 
			m_wireMaterial, glm::vec4(110.0f / 255.0f, 255.0f / 255.0f, 1, 1));
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
	float m_wireScale = 1.008f;
};