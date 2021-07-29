#include "Input.h"
#include "Application.h"

#include <GLFW/glfw3.h>


//returns true if the key is being held this frame regardless 
//of whether it was just pressed or is being held down
bool Input::IsKeyPressed(int keyCode)
{
	GLFWwindow* window = Application::GetWindow();
	int isPressed = glfwGetKey(window, keyCode);
	return isPressed == GLFW_PRESS || isPressed == GLFW_REPEAT;
}

//returns true if the mouse button is being held this frame regardless 
//of whether it was just pressed or is being held down
bool Input::IsMouseButtonPressed(int button)
{
	GLFWwindow* window = Application::GetWindow();
	int isPressed = glfwGetMouseButton(window, button);
	return isPressed == GLFW_PRESS;
}

//returns a vec2 representing the mouse position in screen coordinates
glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* window = Application::GetWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return { (float)xpos, (float)ypos };
}

//retrieves only the x component of the mouse component
float Input::GetMouseX()
{
	return GetMousePosition().x;
}

//retrieves only the y component of the mouse component
float Input::GetMouseY()
{
	return GetMousePosition().y;
}

/*allows to lock/unlock the cursor in the center of the screen. 
  if the cursor is locked it will also be hidden. 
  when being unlocked the cursor will be made visible again
*/
void Input::SetLockCursor(bool value)
{
	unsigned int valueEnum = value ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode(Application::GetWindow(), GLFW_CURSOR, valueEnum);
}
