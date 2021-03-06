#pragma once
#include "../Core/Script.h"
#include "../Rendering/Renderer3D.h"

class CameraControllerTestScene : public Script
{
public:
	void OnRender()
	{
		Renderer3D::DrawVoxel({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawVoxel({ 3, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawVoxel({ -3, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		Renderer3D::DrawVoxel({ 0, 2, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
	}
};