#ifndef _MYTIME_H_
#define _MYTIME_H_

//static class which provides time related functions
class Time
{
	friend class Application;
public:

	//return the actual time in seconds since the application 
	//was created (equivalent to calling glfwGetTime)
	static float GetTime();

	//returns time in between frames in seconds
	static float GetDeltaTime();

private:
	//function which calculates the time between this call on it and it's last call
	static void UpdateTime();

	static float s_timeCurrFrame;
	static float s_timeLastFrame;
};

#endif
