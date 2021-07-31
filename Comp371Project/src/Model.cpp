#include "Model.h"


std::shared_ptr<OpenGLCubeMap> Model::modelTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/ShinyMetal.jpg");
std::shared_ptr<OpenGLCubeMap> Model::wallTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");