#pragma once
#include <memory>
#include <vector>
#include "../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"

class Script;

/*singleton class responsible for the life duration of the program and will 
  also contain all the scripts and the window of the application.
*/
class Application
{
public:
	/*constructor/destructor of the Application, will take care of all the 
	glfw and glew initialization as well as termination and memory 
	clean up of the scripts in the application
	*/
	Application();
	~Application();

	//used to start and end the Application
	static void Init();
	static void Shutdown();

	//run the application loop
	static void Run();

	//can call to stop application through code
	static void Exit();

	//returns the window of the application
	static GLFWwindow* GetWindow() { return GetApplication().m_window; }

	//add and remove scripts from the application, all application must be heap allocated
	static void AddScript(Script* s);
	static void RemoveScript(Script* s);

	static void SetBackgroundColor(glm::vec4 color) { GetApplication().m_backgroundColor = color; }

private:
	static Application* s_instance;
	
	//internal function to retrieve the application instance
	static Application& GetApplication();
	
	//helper functions to call the different functions of the script in the application loop
	void CallOnStartScripts();
	void CallOnRenderScripts();
	void CallOnUpdateScripts();

	bool m_isRunning;
	std::vector<Script*> m_scripts;
	GLFWwindow* m_window;
	glm::vec4 m_backgroundColor = glm::vec4(0, 0, 0, 1);
};