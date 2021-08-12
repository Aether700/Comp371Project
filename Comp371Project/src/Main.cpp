#define GLEW_STATIC
#include "ModelManager.h"

#include "Core/Application.h"
#include "Core/Script.h"

#include "Game/HyperCubeGame.h"

/*Task seperation
* 
* Lighting and Shadows (Jamil Hanachian, Alec, Madelaine, Jean)
* 
* Textures framework (Jamil Hanachian, Madelaine)
* 
* Grid Tiles (Jean)
* 
* World Transform (Jean)
* 
* Camera Tilt, model reset (Alec)
* 
* Translation and rotations of the model (Jean)
* 
* Window resize (Jamil Hanachian)
* 
*/

/*all textures can be been found on the following webpages:
* - bricks: https://traditionalbrickandstone.co.uk/product/aldeburgh-blend/
* - tiles: https://creativemarket.com/ThreeDiCube/2753544-Paving-tiles-seamless-texture
* - shinyMetal: https://www.deviantart.com/grevenlx/art/Metal-Texture-PLUS-Metal-Grid-9940521
* - block: https://www.istockphoto.com/photo/spaceship-hull-texture-or-pattern-seamless-scifi-panels-futuristic-illustration-gm1163094202-319256293
* - palmTree: https://hdwallpaperim.com/vaporwave-1980s-texture-neon/
/

/*mp3 files obtained location listed below
* - breakout.mp3: https://learnopengl.com/audio/in-practice/breakout/breakout.mp3
*/
int main()
{
	Application::Init();

	//Add Scripts here   

	Application::AddScript(new HyperCubeGame());

	///////////////////

	Application::Run();

	Application::Shutdown();
}
