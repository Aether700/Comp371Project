#pragma once

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

protected:
	Script() { OnAwake(); }
};