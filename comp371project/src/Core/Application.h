#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
#include <vector>
#include <string>


#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include "../Rendering/Camera.h"


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
	Application(const std::string& windowName, unsigned int width, unsigned int height);
	~Application();

	//used to start and end the Application
	static void Init(const std::string& windowName = "Comp 371 Project", unsigned int width = 1024, unsigned int height = 768);
	static void Shutdown();

	//run the application loop
	static void Run();

	//can call to stop application through code
	static void Exit();

	//returns the window of the application
	static GLFWwindow* GetWindow() { return GetApplication().m_window; }
	static std::shared_ptr<Camera> GetCamera() { return GetApplication().m_camera; }

	//add and remove scripts from the application, all application must be heap allocated
	static void AddScript(Script* s);
	static void RemoveScript(Script* s);

	//utils functions used to change opengl settings
	static void SetBackgroundColor(glm::vec4 color) { GetApplication().m_backgroundColor = color; }
	static void SetPointSize(float diameter);

private:
	static Application* s_instance;
	
	//internal function to retrieve the application instance
	static Application& GetApplication();

	//helper functions to call the different functions of the script in the application loop
	void CallOnStartScripts();
	void CallOnRenderScripts();
	void CallOnUpdateScripts();

	//intermediate function which calls 
	//Script::HandleScrollEvent on every script until one returns true
	static void HandleScrollEvent(GLFWwindow* w, double xScroll, double yScroll);

	bool m_isRunning;
	std::vector<Script*> m_scripts;
	GLFWwindow* m_window;
	glm::vec4 m_backgroundColor = glm::vec4(0, 0, 0, 1);
	std::shared_ptr<Camera> m_camera;
};

#endif
