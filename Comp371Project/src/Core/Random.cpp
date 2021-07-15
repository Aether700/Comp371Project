#include "Random.h"
#include <time.h>
#include <random>

/*initializes the seed of the rand function using the standard srand function
  by passing it the current time (time(NULL) returns the current time)
*/
void Random::Init()
{
	srand((unsigned int)time(nullptr));
}

//wrapper function for the c function rand
int Random::GetInt() { return rand(); }

/* takes the value returned by rand as a float and divides it by
   32000.0f (this specific value was obtained through trial and errors)
*/
float Random::GetFloat()
{
	return (float)rand() / 32000.0f;
}