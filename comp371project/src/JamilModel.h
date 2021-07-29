#ifndef _JAMIL_MODEL_H_
#define _JAMIL_MODEL_H_

#include "Model.h"

//Jamil Hirsh's model (there are two Jamil)
class JamilModel : public Model
{
public:
	JamilModel()
	{
		for (auto& transform : cubes)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}

		for (auto& transform : walls)
		{
			transform = std::make_shared<Transform>();
			transform->SetParent(GetModelTransform());
		}
	}

	

	void OnRender()
	{
		for (auto& transform : cubes)
		{
			RenderCube(transform->GetTransformMatrix(), color);
		}

		for (auto& transform : walls)
		{
			Renderer3D::DrawVoxel(transform->GetTransformMatrix());
		}
	}


private:
	std::shared_ptr<Transform> cubes[20];
	std::shared_ptr<Transform> walls[40];
	glm::vec4 color = {0, 1, 1,0.5 };



	
	void OnStart()
	{

		for (int i = 0; i < 10; i++) {
			
			//set cubs 
			cubes[i]->position = { i,  0,  0 };
			if(i%2!=0)
			cubes[i+10]->position = { i,  -1,  0 };
			if(i%2 ==0)
			cubes[i + 10]->position = { i,  1,  0 };

			
			
			
			//set walls 
			if (i % 2 != 0)
			walls[i]->position = { i, 1, -5 };
			if (i % 2 == 0)
			walls[i+10]->position = { i, -1, -5 };
			walls[i + 20]->position = { i, 2, -5 };
			walls[i + 30]->position = { i, -2, -5 };

		}
		
		
		for (auto& transform : cubes)
		{
			transform->position.y += 2;
		}

		
		
	



		for (auto& transform : walls)
		{
			transform->position.y += 2;
		}
	}
};

#endif
