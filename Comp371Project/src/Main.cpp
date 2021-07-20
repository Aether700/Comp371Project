#define GLEW_STATIC
#include "Grid.h"
#include "Axes.h"
#include "ModelManager.h"

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

int main()
{
	Application::Init();

	//Add Scripts here   
	Application::AddScript(new Axes());
	Application::AddScript(new Grid());
	Application::AddScript(new ModelManager());
	///////////////////

	Application::Run();

	Application::Shutdown();
}
