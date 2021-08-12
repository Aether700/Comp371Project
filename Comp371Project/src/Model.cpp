#include "Model.h"
#include "Core/Debug.h"

std::shared_ptr<OpenGLCubeMap> Model::modelTexture;
std::shared_ptr<OpenGLCubeMap> Model::wallTexture;

void Model::Init()
{
	modelTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG"); //("Resources/Textures/ShinyMetal.jpg");
	wallTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/ShinyMetal.jpg");
}