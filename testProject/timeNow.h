#ifndef TIMENOW_H
#define TIMENOW_H

#include "sys/time.h"

inline double timeNow( )
    {
    struct timeval newTime;
    gettimeofday(&newTime, 0);

    long int secs =  newTime.tv_sec;
    long int usecs = newTime.tv_usec;

    double deltaTime = secs + ( usecs / 1000000.0f );

    return deltaTime;
    }

#endif // TIMENOW_H
