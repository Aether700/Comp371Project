#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Application.h"
#include "Script.h"

#include <iostream>

Application* Application::s_instance = nullptr;

void GlfwErrorCallbackHandler(int errCode, const char* message)
{
	std::cout << "GLFW error: (" <<  errCode << ") " << message << "\n";
}

void Application::Init()
{
	s_instance = new Application();
}

void Application::Shutdown()
{
	delete s_instance;
}

void Application::Run()
{
	Application& app = GetApplication();
	app.m_isRunning = true;

	app.CallOnStartScripts();

	while (app.m_isRunning && !glfwWindowShouldClose(app.m_window))
	{
		app.CallOnRenderScripts();

		glfwPollEvents();
		glfwSwapBuffers(app.m_window);
		
		app.CallOnUpdateScripts();
	}
}

void Application::Exit()
{
	GetApplication().m_isRunning = false;
}

void Application::AddScript(Script* s)
{
	GetApplication().m_scripts.push_back(s);
}

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

Application::Application()
{
	if (glfwInit() == GLFW_FALSE)
	{
		throw "GLFW could not be initialized\n";
	}

	m_window = glfwCreateWindow(1300, 800, "Comp 371 Project", nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		throw "GLFWwindow could not be created\n";
	}

	glfwMakeContextCurrent(m_window);

	glfwSetErrorCallback(&GlfwErrorCallbackHandler);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw "GLEW could not be initialized\n";
	}
}

Application::~Application()
{
	glfwTerminate();

	for (Script* s : m_scripts)
	{
		delete s;
	}
}

Application& Application::GetApplication()
{
	return *s_instance;
}

void Application::CallOnStartScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnStart();
	}
}

void Application::CallOnRenderScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnRender();
	}
}

void Application::CallOnUpdateScripts()
{
	for (Script* s : m_scripts)
	{
		s->OnUpdate();
	}
}