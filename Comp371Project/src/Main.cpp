#define GLEW_STATIC
#include "TriangleTest.h"
#include "CubeRenderingTest.h"
#include "Core/Application.h"
#include "Core/Script.h"

int main()
{
	Application::Init();

	//Add Scripts here   
	//Application::AddScript(new TriangleTest());
	Application::AddScript(new CubeRendereringTest());
	///////////////////

	Application::Run();

	Application::Shutdown();
}
