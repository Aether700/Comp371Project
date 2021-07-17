#define GLEW_STATIC
#include "Tests/TriangleTest.h"
#include "Tests/CubeRenderingTest.h"
#include "Tests/EmptySquareTest.h"
#include "Tests/ParentTransformTest.h"
#include "Tests/CameraControllerTestScene.h"

#include "JamilHModel.h"
#include "Grid.h"
#include "JeanModel.h"
#include "AlecModel.h"
#include "Axes.h"
#include "Core/Application.h"
#include "Core/Script.h"

#include "Core/Debug.h"

int main()
{
	Application::Init();

	//Add Scripts here   
	//Application::AddScript(new JamilHModel());
	//Application::AddScript(new JeanModel());
	Application::AddScript(new AlecModel());
	Application::AddScript(new Axes());
	//Application::AddScript(new Grid());
	///////////////////

	Application::Run();

	Application::Shutdown();

	/*
	if (glfwInit() == GLFW_FALSE)
	{
		throw "GLFW could not be initialized\n";
	}

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "triangle practice", nullptr, nullptr);


	if (window == nullptr)
	{
		glfwTerminate();
		throw "GLFWwindow could not be created\n";
	}

	glfwMakeContextCurrent(window);

	
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw "GLEW could not be initialized\n";
	}

	float pos[] = {
		 0.0f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	unsigned int vertexArray;
	glCreateVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	unsigned int vertexBuffer;
	glCreateBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	//send data to gpu
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, pos, GL_STATIC_DRAW);

	//tell gpu how to read the data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	
	
	const char* vertexShaderSrc =
		"#version 400\n"
		"in vec3 pos;"
		"void main()"
		"{"
		"gl_Position = vec4(pos, 1);"
		"}";

	const char* fragmentShaderSrc =
		"#version 400\n"
		"out vec4 outColor;"
		"void main()"
		"{"
		"outColor = vec4(0.8f, 0f, 0.5f, 1f);"
		"}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
	glCompileShader(vertexShader);

	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSrc, nullptr);
	glCompileShader(fragShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	glClearColor(0, 0, 0, 1);
	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}*/
}
