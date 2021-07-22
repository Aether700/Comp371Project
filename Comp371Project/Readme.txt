	Comp 371 Project

Controls:

- wasd to move the camera around
- mouse movement to rotate the camera and look around 
- ESC to free the cursor
- left click the window to lock the cursor and rotate the camera again
- Scroll down to zoom in and scroll up to zoom out
- R to reset the camera transform and FOV
- use keys 1 through 5 to select a model (defaults to the cube selected by 1)
- press C to toggle movement mode for the models between translation and rotation (defaults to rotation)
- use arrow keys to rotate/move the currently selected model, the transformation 
	applied depends on the current movement mode selected
- press P, T and L to change the rendering primitive used to render the selected 
	model (P = points, T= triangles, L = lines)
-use U and J to scale up and down the selected model
-use Z to attempt to shuffle the model (some models might not support this operation)


Features:

The whole project runs on a 3D batch renderer coded with opengl. Every draw command from 
the renderer sorts the data to draw per primitve used and then does a single draw call for 
each primitive used unless the batch renderer's capacity has been exeeded in which case the 
filled in "batch" is flushed and drawn to the screen before continuing to process input otherwise 
the renderer is flushed at the end of every frame.

The models are color coded as following

Jamil Hanchian: Red
Jean Carlos Ng Lee: RGB:52, 107, 194(Denim)
Madelaine: Pink
Alec: Green
Jamil Hirsh: light blue

The tasks have been seperated as following

- Application/Script framework, camera + camera controller, batch renderer (Jamil Hanachian)
- Coordinate axis (Alec, Jamil Hirsh)
- Model Manager (Madelaine, Jamil Hanachian)
- Grid (Jean)

