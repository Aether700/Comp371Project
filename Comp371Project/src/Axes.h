#pragma once
#include "Core/Script.h"
#include "Rendering/Renderer3D.h"

class Axes : public Script
{
public:

	Axes()
	{
	}

	~Axes() {}

	void OnStart()
	{

	}

	void OnRender()
	{
		glm::vec3 origin = { 0,0,0 };
		//x
		//Renderer3D::DrawLine(tr_x.GetTransformMatrix(), { 1,0,0,0 });
		Renderer3D::DrawLine(origin + glm::vec3{2.5,0,0}, { 0,glm::radians(0.0f),0 }, { 5,0,0 }, { 1,0,0,0 });


		//y
		Renderer3D::DrawLine(origin + glm::vec3{0,2.5,0}, { 0,0,glm::radians(90.0f) }, { 5,0,0 }, { 0,1,0,0 });

		//z
		Renderer3D::DrawLine(origin + glm::vec3{ 0,0,2.5 }, { 0,glm::radians(90.0f),0 }, { 5,0,0 }, { 0,0,1,0 });
	}

private:

};
