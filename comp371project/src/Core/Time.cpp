#include "Time.h"

#include <GLFW/glfw3.h>



float Time::s_timeCurrFrame = 0.0f;
float Time::s_timeLastFrame = 0.0f;

//return the actual time in seconds since the application 
//was created (equivalent to calling glfwGetTime)
float Time::GetTime()
{
	return (float)glfwGetTime();
}

//returns time in between frames in seconds
float Time::GetDeltaTime()
{
	return s_timeCurrFrame;
}

//function which calculates the time between this call on it and it's last call
void Time::UpdateTime()
{
	float temp = GetTime();
	s_timeCurrFrame = temp - s_timeLastFrame;
	s_timeLastFrame = temp;
}
