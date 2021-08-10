#include "GameModel.h"
#include "../Core/Debug.h"

std::shared_ptr<OpenGLCubeMap> GameModel::modelTexture = nullptr;
std::shared_ptr<OpenGLCubeMap> GameModel::wallTexture = nullptr;

void GameModel::Init()
{
	modelTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/Bricks.PNG");
	wallTexture = std::make_shared<OpenGLCubeMap>("Resources/Textures/block.jpg");