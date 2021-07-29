#ifndef _RANDOM_H_
#define _RANDOM_H_

/* Basic rudementary Random class to get random ints and floats
*/
class Random
{
public:
	/*	Initializes the Random class by setting the seed of the c
		function rand to the current time allowing pseudo random number generation

		Needs to be used at the start of the program to set the seed. Do not use more than once.
		only use when starting your program
	*/
	static void Init();

	/*	returns a random integer between 0 and RAND_MAX
		(a maximum value defined by c libraries which is at least 32767).
		Note that RAND_MAX cannot be returned

		returns: a random integer between 0 and RAND_MAX (RAND_MAX not included)
	*/
	static int GetInt();

	/* returns a random float number between 0 and 1 (one not included)

		returns: a random float number between 0 and 1 (one not included)
	*/
	static float GetFloat();

};

#endif
