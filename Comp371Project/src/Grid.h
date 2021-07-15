#pragma once


class Grid : public Script
{
public:
	void OnRender()
	{
		for (int x = -50; x < 50; x++) // started at -50 since it need to be center at origin assuming(0,0) being origin
		{
			for (int z = -50; z < 50; z++)
			{
				Renderer3D::DrawWireSquare({ x, -0.5, z }, { 1.5708, 0, 0 }, { 1, 1, 1 });
				//1.5708rad= 90degree since size of the square is 1 and by rotating it 90 degree it will be at heigh (1,0.5,1) whihc is why the (x,-0.5, z)
			}
		}

	}

};
