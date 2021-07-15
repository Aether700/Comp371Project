#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Rendering/Renderer3D.h"
#include "../Rendering/CameraController.h"
#include "Application.h"
#include "Script.h"
#include "Time.h"
#include "Debug.h"

#include <iostream>

Application* Application::s_instance = nullptr;

void GlfwErrorCallbackHandler(int errCode, const char* message)
{
	std::cout << "GLFW error: (" <<  errCode << ") " << message << "\n";
}

void WindowResizeEvent(GLFWwindow* w, int width, int height)
{
	Application::GetCamera()->UpdateAspectRatio();
	glViewport(0, 0, width, height);
}

void Application::Init(const std::string& windowName, unsigned int width, unsigned int height)
{
	s_instance = new Application(windowName, width, height);
	Debug::CheckOpenGLError();
	s_instance->m_camera->UpdateAspectRatio();
	AddScript(new CameraController());
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
		//reset stats for the frame
		Renderer3D::ResetStats();

		Time::UpdateTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(app.m_backgroundColor.r, app.m_backgroundColor.g, app.m_backgroundColor.b, app.m_backgroundColor.a);

		Renderer3D::BeginScene();
		app.CallOnRenderScripts();
		Renderer3D::EndScene();

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

void Application::SetPointSize(float diameter)
{
	assert(diameter > 0.0f);
	glPointSize(diameter);
}

Application::Application(const std::string& windowName, unsigned int width, unsigned int height)
{
	if (glfwInit() == GLFW_FALSE)
	{
		throw "GLFW could not be initialized\n";
	}

	m_window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		throw "GLFWwindow could not be created\n";
	}

	glfwMakeContextCurrent(m_window);

	glfwSetErrorCallback(&GlfwErrorCallbackHandler);

	glfwSetWindowSizeCallback(m_window, &WindowResizeEvent);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw "GLEW could not be initialized\n";
	}

	m_camera = std::make_shared<Camera>();
	Renderer3D::Init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS);

	glPointSize(5.0f);
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