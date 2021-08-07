#pragma once
#include "Model.h"

class AlecModel : public Model
{
public:
	AlecModel(std::shared_ptr<Transform> worldTransform) : Model(worldTransform)
	{
		for (auto& transform : m_cubeArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}

		for (auto& transform : m_wallArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetWallTransform());
		}

		//the wireframe glow effect is partially achieved by ignoring lighting
		m_wireframe_glow_material = Material(true);

		//light should follow world movement
		m_lightTransform.SetParent(worldTransform);
	}

	void OnStart()
	{
		//Model
		//set up model by arranging individual cube's transforms
		m_cubeArr[0]->position = { 0,  0,  0 };
		m_cubeArr[1]->position = { -1,  0,  0 };
		m_cubeArr[2]->position = { -2, 0,  0 };
		m_cubeArr[3]->position = { -1,  -1,  0 };
		m_cubeArr[4]->position = { 0,  1,  0 };
		m_cubeArr[5]->position = { 1,  0,  0 };
		m_cubeArr[6]->position = { 1,  1,  0 };
		m_cubeArr[7]->position = { 2, 0,  0 };
		m_cubeArr[8]->position = { 2, 1,  0 };
		m_cubeArr[9]->position = { 2, -1,  0 };
		m_cubeArr[10]->position = { 0,  0, -1 };
		m_cubeArr[11]->position = { -1, -1, -1 };
		m_cubeArr[12]->position = { 0,  1, -1 };
		m_cubeArr[13]->position = { 2, -1, -1 };


		//Wall
		//left side
		m_wallArr[0]->position = { -3, -2, -4 };
		m_wallArr[1]->position = { -3, -1, -4 };
		m_wallArr[2]->position = { -3, 0, -4 };
		m_wallArr[3]->position = { -3, 1, -4 };
		m_wallArr[4]->position = { -3, 2, -4 };

		//bottom
		m_wallArr[5]->position = { -2, -2, -4 };
		m_wallArr[6]->position = { -1, -2, -4 };
		m_wallArr[7]->position = { 0, -2, -4 };
		m_wallArr[8]->position = { 1, -2, -4 };
		m_wallArr[9]->position = { 2, -2, -4 };

		//top
		m_wallArr[10]->position = { -2, 2, -4 };
		m_wallArr[11]->position = { -1, 2, -4 };
		m_wallArr[12]->position = { 0, 2, -4 };
		m_wallArr[13]->position = { 1, 2, -4 };
		m_wallArr[14]->position = { 2, 2, -4 };

		//right
		m_wallArr[15]->position = { 3, -2, -4 };
		m_wallArr[16]->position = { 3, -1, -4 };
		m_wallArr[17]->position = { 3, 0, -4 };
		m_wallArr[18]->position = { 3, 1, -4 };
		m_wallArr[19]->position = { 3, 2, -4 };

		//inside
		m_wallArr[20]->position = { -2, -1, -4 };
		m_wallArr[21]->position = { -2, 1, -4 };
		m_wallArr[22]->position = { -1, 1, -4 };
		m_wallArr[23]->position = { 0, -1, -4 };
		m_wallArr[24]->position = { 1, -1, -4 };


	}

	void OnRender()
	{
		for (auto& transform : m_cubeArr)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (auto& transform : m_wallArr)
		{
			RenderWall(transform->GetTransformMatrix());
		}

		// Light should only appear and cast light/shadows if this model is selected
		if (IsSelected())
		{
			glm::mat4 tr_matrix = m_lightTransform.GetTransformMatrix();
			Renderer3D::AddPointLight(glm::vec3(tr_matrix[3][0], tr_matrix[3][1], tr_matrix[3][2]));
		}
	}

	//Set light to be above model
	//This happens only at the beginning of the program, so the light does move with the world but not with the model
	void SetLightPos()
	{
		m_lightTransform.position = (glm::vec3)(GetModelTransform()->GetTransformMatrix())[3] + glm::vec3{ 0, 30, 0 };
	}

protected:
	virtual void RenderCubeWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		
		if(GetRenderingPrimitive() == RenderingPrimitive::Points)
		{
			Renderer3D::DrawPointCube(transform, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Lines)
		{
			Renderer3D::DrawWireCube(transform, color);
		}
		else if (GetRenderingPrimitive() == RenderingPrimitive::Triangles)
		{
			Renderer3D::DrawVoxel(transform, modelTexture, 1, color);
		}

	}

	virtual void RenderWallWithTexture(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 }) override
	{
		Renderer3D::DrawVoxel(transform, wallTexture, 1, color);
	}

	//Implement glowing boundary effect with wireframe
	//Called when model is selected, once per cube drawn
	virtual void DrawOnSelected(const glm::mat4& transform, const glm::vec4& color) 
	{ 
		Renderer3D::DrawWireCube(glm::scale(transform, { 1.007, 1.007, 1.007 }), m_wireframe_glow_material, glm::vec4{ 1 } - color + glm::vec4{ 0, 0, 0, 1 });
	}

private:



	std::shared_ptr<Transform> m_cubeArr[14];
	std::shared_ptr<Transform> m_wallArr[25];

	glm::vec4 color = { 0.1, 0.9, 0.1, 1 };

	Transform m_lightTransform;

	Material m_wireframe_glow_material;
};