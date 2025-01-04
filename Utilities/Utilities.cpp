// Utilities.cpp : Defines the exported functions for the DLL application.
//

#include "Utilities.h"

// This is the constructor of a class that has been exported.
// see Utilities.h for the class definition
CUtilities::CUtilities() {}

void CUtilities::start() {
   startTime = gettimeofday();
}

double CUtilities::stop() {
	time_t endTime = gettimeofday();
	time_t seconds;// , nseconds;
    double duration;
    seconds  = endTime - startTime;
    //nseconds = endTime.tv_usec - startTime.tv_usec;
	duration = (double)seconds;// / 1000.0;
 	return duration;
}

time_t CUtilities::gettimeofday () {
	return std::time ( NULL );
}




