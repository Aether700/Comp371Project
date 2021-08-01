#define GLEW_STATIC
#include "Axes.h"
#include "ModelManager.h"

#include "Tests/AlecPointLightTest.h"
#include "Tests/TextureTest.h"
#include "Tests/RendererDirectionalLightTest.h"

#include "Core/Application.h"
#include "Core/Script.h"

#include "Core/Debug.h"

/*Task seperation
* 
* Application/Script framework, camera + camera controller, batch renderer (Jamil Hanachian)
* 
* Coordinate axis (Alec, Jamil Hirsh)
* 
* Model Manager (Madelaine, Jamil Hanachian)
* 
* Grid (Jean)
* 
*/

/*all textures can be been found on the following webpages:
* - bricks: https://traditionalbrickandstone.co.uk/product/aldeburgh-blend/
* - tiles: https://creativemarket.com/ThreeDiCube/2753544-Paving-tiles-seamless-texture
* - shinyMetal: https://www.deviantart.com/grevenlx/art/Metal-Texture-PLUS-Metal-Grid-9940521
*/
int main()
{
	Application::Init();

	//Add Scripts here   

	//Application::AddScript(new Axes());
	//Application::AddScript(new ModelManager());
	//Application::AddScript(new AlecPointLightTest());
	Application::AddScript(new RendererDirectionalLightTest());

	///////////////////

	Application::Run();

	Application::Shutdown();
}
