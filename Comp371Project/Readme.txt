	Comp 371 Project

Gameplay:
- wasd to rotate the model
- hold space to increase the speed at which the model travels towards the wall
- Shift + C to toggle debug mode which allows free camera movement
- esc to pause/resume the game

Controls:
Camera (only works when in debug mode)
- wasd to move the camera around
- space to move the camera up and left Ctrl to move it down
- mouse movement to rotate the camera and look around 
- Scroll down to zoom in and scroll up to zoom out
- esc to pause/resume the game


Features:

The whole project runs on a 3D batch renderer coded with opengl. Every draw command from 
the renderer sorts the data to draw per primitve used and then does a single draw call for 
each primitive. The renderer is flushed at the end of every frame.


NEW FEATURE/FIX ADDED AFTER FEEDBACK
- Added a Game main menu 
- Added a pause menu
- Additions to game 
	- as more models are successfully rotated to fit into the wall the speed at which the model 
		travels towards the wall increases
	- combos are allocated for multiple consecutive models fitted through the wall as well as an increasing 
		multiplier which increases the amount of points gained per model
	- the score no longer resets when the model collides with the wall. Instead the combo and 
		score multiplier and model speed are reset
	- the combo and multiplier can be seen in the UI when currently active

- Improve drop animation so the model drops longer and faster
- Make score/timer bigger
- The fit animation now moves the model through the wall faster and 
	longer so the model can be seen going completely through the wall

- Light cube no longer appear in the scene unless in debug mode
- Light cube now match their respective light color
- Camera angle has been pulled back from the model so it sees more of the scene

- UI elements will now resize/reposition themselves depending on the window size
- flickering outline of the cubes of both models and walls have been fixed.
- the color of the outlines for the cubes in models and walls has been changed 
	to have a bigger contrast and better highlight the geometry