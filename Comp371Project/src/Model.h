#pragma once
#include "Core/Script.h"
#include "Rendering/Renderer3D.h"

enum class RenderingPrimitive
{
	Triangles,
	Lines,
	Points
};

//base class for every model
class Model : public Script
{
public:

	static void Init();

	virtual ~Model() { }

	
	std::shared_ptr<Transform>& GetTransform() { return m_transform; }
	std::shared_ptr<Transform>& GetModelTransform() { return m_modelTransform; }
	std::shared_ptr<Transform>& GetWallTransform() { return m_wallTransform; }

	void SetRenderingPrimitive(RenderingPrimitive primitive) { m_primitive = primitive; }
	void ToggleTexture() { istextureOn = !istextureOn; }
	RenderingPrimitive GetRenderingPrimitive() const { return m_primitive; }

	bool IsSelected() const { return m_isSelected; }
	void Select() { m_isSelected = true; }
	void Unselect() { m_isSelected = false; }

	//shuffles the model or displays a message in the console indicating that this model does not support shuffling
	virtual void Shuffle() 
	{
		std::cout << "This model does not support the shuffle operation\n";
	}

protected:
	Model(std::shared_ptr<Transform> worldTransform) : m_modelTransform(std::make_shared<Transform>()), 
		m_wallTransform(std::make_shared<Transform>()), m_transform(std::make_shared<Transform>())
	{
		m_modelTransform->SetParent(m_transform);
		m_wallTransform->SetParent(m_transform);
		m_transform->SetParent(worldTransform);

	}

	//rendering function to use inside the models so that we can change the rendering 
	//primitives used to render them
	void RenderCubeNoTexture(const glm::mat4& transform, const glm::vec4& color = {1, 1, 1, 1})
	{
		if (m_primitive == RenderingPrimitive::Triangles)
		{
			Renderer3D::DrawVoxel(transform, color);
		}
		else if (m_primitive == RenderingPrimitive::Lines)
		{
			Renderer3D::DrawWireCube(transform, color);
		}
		else if (m_primitive == RenderingPrimitive::Points)
		{
			Renderer3D::DrawPointCube(transform, color);
		}
	}

	void RenderWallNoTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		Renderer3D::DrawVoxel(transform, color);
	}

	void RenderCube(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		if (istextureOn)
		{
			RenderCubeWithTexture(transform, color);
		}
		else
		{
			RenderCubeNoTexture(transform, color);
		}

		if (m_isSelected)
		{
			DrawOnSelected(transform, color);
		}
	}

	void RenderWall(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		if (istextureOn)
		{
			RenderWallWithTexture(transform, color);
		}
		else
		{
			RenderWallNoTexture(transform, color);
		}
	}

	virtual void RenderCubeWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		RenderCubeNoTexture(transform, color);
	}

	virtual void RenderWallWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 })
	{
		RenderCubeNoTexture(transform, color);
	}

	//is called when the model is selected, is used to draw the outline of the selected model
	//by default does nothing so if it is not implemented nothing breaks
	virtual void DrawOnSelected(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) { }

	static std::shared_ptr<OpenGLCubeMap> wallTexture;
	static std::shared_ptr<OpenGLCubeMap> modelTexture;


private:
	std::shared_ptr<Transform> m_transform;
	std::shared_ptr<Transform> m_modelTransform;
	std::shared_ptr<Transform> m_wallTransform;
	RenderingPrimitive m_primitive = RenderingPrimitive::Triangles;
	bool istextureOn = false;
	bool m_isSelected = false;
};