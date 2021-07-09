#pragma once
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"

class Input
{
public:
	static bool IsKeyPressed(int keyCode);
	static bool IsMouseButtonPressed(int button);
	static glm::vec2 GetMousePosition();
	static float GetMouseX();
	static float GetMouseY();
};
