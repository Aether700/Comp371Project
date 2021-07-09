#include "Input.h"
#include "Application.h"
#include "../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"

bool Input::IsKeyPressed(int keyCode)
{
	GLFWwindow* window = Application::GetWindow();
	int isPressed = glfwGetKey(window, keyCode);
	return isPressed == GLFW_PRESS || isPressed == GLFW_REPEAT;
}


bool Input::IsMouseButtonPressed(int button)
{
	GLFWwindow* window = Application::GetWindow();
	int isPressed = glfwGetMouseButton(window, button);
	return isPressed == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* window = Application::GetWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return { (float)xpos, (float)ypos };
}

float Input::GetMouseX()
{
	return GetMousePosition().x;
}

float Input::GetMouseY()
{
	return GetMousePosition().y;
}