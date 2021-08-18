	Comp 371 Project

Gameplay:
- wasd to rotate the model
- hold space to increase the speed at which the model travels towards the wall
- Shift + C to toggle debug mode which allows free camera movement
- esc to pause the game

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


NEW FEATURE/FIX ADDED AFTER FEEDBACK
- Added a Game main menu 
- Added a pause menu
- Additions to game (speed up + combos + pause menu)

- Improve drop animation
- Make score/timer bigger
- Game pacing the speed at which the model movie change throught the the game given the player successuflly fit the model into the wall

- Light cube no longer appear in the scene
- Light cube should match light color
- Object fall down for longer period
- The object pass through wall completly and stay longer
- Camera angle sees scene/can move to see more of the scene
- Keep camera more fixed / staying back inorder to keep showing scene

- UI elements no longer is affected by the size of the window and window resizing
- Wirecube illumination cause flig a bit glitchy / depth-fightin and change their color for higher contras