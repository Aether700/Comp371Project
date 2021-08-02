#pragma once
#include "../Rendering/Renderer3D.h"
#include "../Core/Script.h"
#include "../Core/Application.h"
#include "../Grid.h"

class LightRadiusTest : public Script
{
public:
	void OnStart()
	{
		Renderer3D::UseShadows(true);
		auto* g = new Grid(std::make_shared<Transform>());
		g->ToggleTexture();
		Application::AddScript(g);
	}

	void OnRender()
	{
		Renderer3D::AddDirectionalLight({0, 5, 0}, {0.1f, -1, 0});
		Renderer3D::AddDirectionalLight({5, 5, 0}, {0.1f, -1, 0});
	}
};