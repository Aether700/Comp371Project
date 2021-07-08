#pragma once


class Time
{
	friend class Application;
public:

	//returns time in sec
	static float GetTime();
	static float GetDeltaTime();

private:
	static void UpdateTime();

	static float s_timeCurrFrame;
	static float s_timeLastFrame;
};
