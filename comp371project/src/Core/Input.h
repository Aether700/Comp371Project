#ifndef _INPUT_H_
#define _INPUT_H_

#include <glm/glm.hpp>

/*static class which provides functions to handle user input
*/
class Input
{
public:
	//returns true if the key is being held this frame regardless 
    //of whether it was just pressed or is being held down
	static bool IsKeyPressed(int keyCode);

	//returns true if the mouse button is being held this frame regardless 
    //of whether it was just pressed or is being held down
	static bool IsMouseButtonPressed(int button);

	//returns a vec2 representing the mouse position in screen coordinates
	static glm::vec2 GetMousePosition();

	//retrieves only the x component of the mouse component
	static float GetMouseX();

	//retrieves only the y component of the mouse component
	static float GetMouseY();

	/*allows to lock/unlock the cursor in the center of the screen.
	  if the cursor is locked it will also be hidden.
	  when being unlocked the cursor will be made visible again
	*/
	static void SetLockCursor(bool value);
};

#endif
