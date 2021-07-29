#ifndef _MODEL_H_
#define _MODEL_H_

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
	virtual ~Model() { }

	std::shared_ptr<Transform>& GetModelTransform() { return m_modelTransform; }

	void SetRenderingPrimitive(RenderingPrimitive primitive) { m_primitive = primitive; }

	//shuffles the model or displays a message in the console indicating that this model does not support shuffling
	virtual void Shuffle() 
	{
		std::cout << "This model does not support the shuffle operation\n";
	}

protected:
	Model() : m_modelTransform(std::make_shared<Transform>()) { }

	//rendering function to use inside the models so that we can change the rendering 
	//primitives used to render them
	void RenderCube(const glm::mat4& transform, const glm::vec4& color = {1, 1, 1, 1})
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

private:
	std::shared_ptr<Transform> m_modelTransform;
	RenderingPrimitive m_primitive = RenderingPrimitive::Triangles;
};

#endif
