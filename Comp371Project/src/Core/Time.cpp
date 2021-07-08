#include "Time.h"
#include "../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"

float Time::s_timeCurrFrame = 0.0f;
float Time::s_timeLastFrame = 0.0f;

float Time::GetTime()
{
	return (float)glfwGetTime();
}

//returns time in sec
float Time::GetDeltaTime()
{
	return s_timeCurrFrame;
}

void Time::UpdateTime()
{
	float temp = GetTime();
	s_timeCurrFrame = temp - s_timeLastFrame;
	s_timeLastFrame = temp;
}
