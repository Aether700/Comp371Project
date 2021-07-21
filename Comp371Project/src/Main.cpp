#define GLEW_STATIC
#include "Grid.h"
#include "Axes.h"
#include "ModelManager.h"

#include "Tests/PointLightTest.h"

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

/*all textures have been found on google image:
* - bricks: https://www.pinterest.co.kr/pin/431078995573218874/
* - tiles: https://creativemarket.com/ThreeDiCube/2753544-Paving-tiles-seamless-texture
*/
int main()
{
	Application::Init();

	//Add Scripts here   
	//Application::AddScript(new Axes());
	//Application::AddScript(new Grid());
	//Application::AddScript(new ModelManager());
	Application::AddScript(new PointLightTest());
	///////////////////

	Application::Run();

	Application::Shutdown();
}
