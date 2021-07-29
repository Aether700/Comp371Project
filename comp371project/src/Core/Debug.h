#ifndef _DEBUG_H_
#define _DEBUG_H_


#define GLEW_STATIC
#include <GL/glew.h>


#include <sstream>
#include <string>
#include <assert.h>
#include <iostream>

//class which contains all the debug functions of the program
class Debug
{
public:

	/*checks if an opengl error has occured before this point and displays all the 
	  error codes for all the errors that have occured since the last call to glGetError()

	  note this function is for debugging purposes only and will crash the program 
	  if an error has occured
	*/
	static void CheckOpenGLError()
	{
		unsigned int errorCode = glGetError();

		if (errorCode == GL_NO_ERROR)
		{
			return;
		}

		std::stringstream ss;

		ss << "OpenGL Errors:\n";

		while (errorCode != GL_NO_ERROR)
		{
			ss << errorCode << "\n";
			errorCode = glGetError();
		}

		std::cerr << ss.str();
		assert(false);
	}
};

#endif
