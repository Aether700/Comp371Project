#define GLEW_STATIC
#include "TriangleTest.h"
#include "CubeRenderingTest.h"
#include "EmptySquareTest.h"

#include "Core/Application.h"
#include "Core/Script.h"


int main()
{
	Application::Init();

	//Add Scripts here   
	//Application::AddScript(new TriangleTest());
	//Application::AddScript(new CubeRendereringTest());
	Application::AddScript(new EmptySquareTest());
	///////////////////

	Application::Run();

	Application::Shutdown();
}
