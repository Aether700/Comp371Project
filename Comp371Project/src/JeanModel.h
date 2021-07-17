#pragma once

#include "Model.h"



class JeanModel : public Model
{
public:

	JeanModel()
	{
		
		for (auto& transform : m_cubeArr)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}
	}

	void OnStart()
	{
		
		m_cubeArr[0]->position = {0, 0, 0};
		m_cubeArr[1]->position = {0, 0, -1};
		m_cubeArr[2]->position = {0, 1, -1};
		m_cubeArr[3]->position = {0, 1, -2};
		m_cubeArr[4]->position = {1, 1, -2};
		m_cubeArr[5]->position = {1, 2, -2};
		m_cubeArr[6]->position = {2, 2, -2};
		m_cubeArr[7]->position = {3, 2, -2};
		m_cubeArr[8]->position = {3, 2, -3};
		m_cubeArr[9]->position = {3, 3, -3};
		m_cubeArr[10]->position = {3, 1, -2};
		m_cubeArr[11]->position = {3, 1, -1};
		m_cubeArr[12]->position = {3, 1, 0};
		m_cubeArr[13]->position = {3, 0, 0};
		m_cubeArr[14]->position = {3, 0, -2};
		m_cubeArr[15]->position = {4, 1, 0};
		
	}

	void OnRender()
	{
		
		for (auto& transform : m_cubeArr)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}
		/*
		Renderer3D::DrawWireSquare({ 0, 0, 0 }, {0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 0, 0, -1 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 0, 1, -1 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 0, 1, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 1, 1, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 1, 2, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 2, 2, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 2, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 2, -3 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 3, -3 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 1, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 1, -1 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 1, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 3, 0, -2 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawWireSquare({ 4, 1, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		*/

	}


private:
	std::shared_ptr<Transform> m_cubeArr[16];

	glm::vec4 color = { 52.0f / 255.0f, 107.0f / 255.0f, 194.0f / 255.0f, 1 };

};