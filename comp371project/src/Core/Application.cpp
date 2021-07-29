#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "../Rendering/Renderer3D.h"
#include "../Rendering/CameraController.h"
#include "Application.h"
#include "Script.h"
#include "Time.h"
#include "Debug.h"
#include "Random.h"

#include <iostream>

Application* Application::s_instance = nullptr;

/*GLFW error callback handler which will display the error to 
  the console and crash the program for debugging purposes

  this function is set as a callback and is called by glfw in case of an error
*/
void GlfwErrorCallbackHandler(int errCode, const char* message)
{
	std::cout << "GLFW error: (" <<  errCode << ") " << message << "\n";
	assert(false);
}

/* Callback called by glfw when the window is resized. 
  this function will update the viewport and the aspect ratio of our camera
*/
void WindowResizeEvent(GLFWwindow* w, int width, int height)
{
	Application::GetCamera()->SetViewportSize(width, height);
	glViewport(0, 0, width, height);
}

/* Initializes the application with the provided window name, width and height
*/
void Application::Init(const std::string& windowName, unsigned int width, unsigned int height)
{
	s_instance = new Application(windowName, width, height);
	//Debug::CheckOpenGLError();
	s_instance->m_camera->SetViewportSize(width, height);
	AddScript(new CameraController());
}

//shuts down the application and clears the memory allocated on the heap
void Application::Shutdown()
{
	delete s_instance;
}

/*main run function which will call the different functions of the scripts, 
  run the Renderer3D::BeginScene and RendererEndScene functions as well as 
  set the background color and clear the different buffers for opengl to 
  work properly and call glfwPollEvents for input management
*/
void Application::Run()
{
	Application& app = GetApplication();
	app.m_isRunning = true;

	app.CallOnStartScripts();

	while (app.m_isRunning && !glfwWindowShouldClose(app.m_window))
	{
		//reset stats for the frame
		Renderer3D::ResetStats();

		Time::UpdateTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(app.m_backgroundColor.r, app.m_backgroundColor.g, app.m_backgroundColor.b, app.m_backgroundColor.a);

		//Renderer3D::BeginScene();
		app.CallOnRenderScripts();
		//Renderer3D::EndScene();

		glfwPollEvents();
		glfwSwapBuffers(app.m_window);
		
		app.CallOnUpdateScripts();
	}
}

//provides a in code way to close the application
void Application::Exit()
{
	GetApplication().m_isRunning = false;
}

//adds the provided script for the application to run. All scripts must be heap allocated
void Application::AddScript(Script* s)
{
	GetApplication().m_scripts.push_back(s);
}

/*removes the provided script for the application 
  to run if it can be found in the application
*/
void Application::RemoveScript(Script* s)
{
	Application& app = GetApplication();

	for (size_t i = 0; i < app.m_scripts.size(); i++)
	{
		if (app.m_scripts[i] == s)
		{
			app.m_scripts.erase(app.m_scripts.begin() + i);
			break;
		}
	}
}

//sets the diameter of the points drawn by opengl
void Application::SetPointSize(float diameter)
{
	assert(diameter > 0.0f);
	glPointSize(diameter);
}

//Application constructor which takes a window name, width and heigh
Application::Application(const std::string& windowName, unsigned int width, unsigned int height)
{
	if (!glfwInit())
	{
		throw "GLFW could not be initialized\n";
	}
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//we create a glfw window with the specified width and height (the default values are 
	//specified in the header file for Application::Init and they correspond to the values 
	//required by the assignment)
	m_window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		throw "GLFWwindow could not be created\n";
	}

	glfwMakeContextCurrent(m_window);

	//set callbacks
	glfwSetErrorCallback(&GlfwErrorCallbackHandler);
	glfwSetWindowSizeCallback(m_window, &WindowResizeEvent);
	glfwSetScrollCallback(m_window, &Application::HandleScrollEvent);

    glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw "GLEW could not be initialized\n";
	}
    std::cout << "version: " << std::endl;
    std::cout <<glGetString(GL_VERSION)  << std::endl;
    
    if(glCreateTextures){
        std::cout << "GLEW: glCreateTextures is suppported" << std::endl;
    }

    
    if(GLEW_VERSION_3_3){
        std::cout << "GLEW: OpenGL Version 3.3 is suppported" << std::endl;
    }
    else{
        std::cout << "GLEW: OpenGL Version 3.3 is NOT suppported" << std::endl;
    }
    
	//we set a virtual camera in the world which will render 
	//the world using perspective view as required by the assignment
	m_camera = std::make_shared<Camera>();
	Renderer3D::Init();
	Random::Init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS);

	//enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPointSize(5.0f);
}

//Application destructor, will delete all the scripts the application contains
Application::~Application()
{
	glfwTerminate();

	for (Script* s : m_scripts)
	{
		delete s;
	}
}

//private helper function used to retrieve the application instance
Application& Application::GetApplication()
{
	return *s_instance;
}

//helper function which calls OnStart on every script in the application
void Application::CallOnStartScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnStart();
	}
}

//helper function which calls OnRender on every script in the application
void Application::CallOnRenderScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnRender();
	}
}

//helper function which calls OnUpdate on every script in the application
void Application::CallOnUpdateScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnUpdate();
	}
}

void Application::HandleScrollEvent(GLFWwindow* w, double xScroll, double yScroll)
{
	for (Script* s : GetApplication().m_scripts)
	{
		if (s->HandleScrollEvent(xScroll, yScroll))
		{
			break;
		}
	}
}
