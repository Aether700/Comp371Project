#ifndef _SCRIPT_H_
#define _SCRIPT_H_

/*basic class which contains functions to be called by the application. 
  This class is meant to be inherited by other classes to be able to 
  provide behavior to the application depending on what scripts are 
  loaded into the application
*/
class Script
{
public:
	virtual ~Script() { }

	//is called when the script is instantiated
	virtual void OnAwake() { }

	//is called right before the application loop begins
	virtual void OnStart() { }

	//is called every frame to render to the screen/window
	virtual void OnRender() { }

	//is called every frame for input management and such
	virtual void OnUpdate() { }

	//returns true if the event has been handled, false otherwise
	virtual bool HandleScrollEvent(double xScroll, double yScroll) { return false; }

protected:
	Script() { OnAwake(); }
};

#endif
