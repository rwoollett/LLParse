#ifndef D_UTILITIES_H_
#define D_UTILITIES_H_

#include <ctime>

class CUtilities
{
	time_t startTime;
    
public:
    CUtilities ();

    void start();
    double stop();
	time_t gettimeofday();

};

#endif /* D_UTILITIES_H_ */