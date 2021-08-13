	Comp 371 Project

Gameplay:
- wasd to rotate the model
- hold space to increase the speed at which the model travels towards the wall
- Shift + C to toggle debug mode which allows free camera movement

Controls:
Camera (only works when in debug mode)
- wasd to move the camera around
- space to move the camera up and left Ctrl to move it down
- mouse movement to rotate the camera and look around 
- ESC to free the cursor
- left click the window to lock the cursor and rotate the camera again
- Scroll down to zoom in and scroll up to zoom out


Features:

The whole project runs on a 3D batch renderer coded with opengl. Every draw command from 
the renderer sorts the data to draw per primitve used and then does a single draw call for 
each primitive. The renderer is flushed at the end of every frame.