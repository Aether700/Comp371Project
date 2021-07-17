#pragma once


class Grid : public Script
{
public:
	void OnRender()
	{
		for (int x = -50; x <= 50; x++) // started at -50 since it need to be center at origin assuming(0,0) being origin
		{
			Renderer3D::DrawLine(m_transform.GetTransformMatrix(), { x, 0, -50 }, {x, 0, 50 });
		}

		for (int z = -50; z <= 50; z++)
		{
			Renderer3D::DrawLine(m_transform.GetTransformMatrix(), { -50, 0, z }, { 50, 0, z });
		}
	}

private:
		Transform m_transform = Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
};
