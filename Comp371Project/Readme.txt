	Comp 371 Project

Controls:

- wasd to move the camera around
- mouse movement to rotate the camera and look around 
- ESC to free the cursor
- left click the window to lock the cursor and rotate the camera again
- Scroll down to zoom in and scroll up to zoom out
- R to reset the camera transform and FOV


Features:

The whole project runs on a 3D batch renderer coded with opengl. Every draw command from 
the renderer sorts the data to draw per primitve used and then does a single draw call for 
each primitive used unless the batch renderer's capacity has been exeeded in which case the 
filled in "batch" is flushed and drawn to the screen before continuing to process input otherwise 
the renderer is flushed at the end of every frame.

The models are color coded as following

Jamil Hanchian: Red

Jean Carlos Ng Lee: RGB:52, 107, 194(Denim)
