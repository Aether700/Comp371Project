#ifndef _GRID_H_
#define _GRID_H_

#include "Core/Script.h"
#include "Rendering/Renderer3D.h"

class Grid : public Script
{
public:
	void OnRender()
	{
		for (int displacement = -50; displacement <= 50; displacement++) // started at -50 since it need to be center at origin assuming(0,0) being origin
		{
			Renderer3D::DrawLine(m_transform.GetTransformMatrix(), { displacement, 0, -50 }, { displacement, 0, 50 });
			Renderer3D::DrawLine(m_transform.GetTransformMatrix(), { -50, 0, displacement }, { 50, 0, displacement });
		}
	}

private:
		Transform m_transform = Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
};

#endif
