#ifndef CILK_EXAMPLE_UTIL_GETTIME_H_INCLUDED
#define CILK_EXAMPLE_UTIL_GETTIME_H_INCLUDED


#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#include <sys/time.h>
#endif


extern "C++"
inline
int example_get_time()
{
#ifdef _WIN32
    return (int) GetTickCount();
    #else
    static timeval* start = NULL;
    struct timezone tzp ={0, 0};
    if (NULL == start) {
      start = (timeval *) malloc(sizeof(timeval));
      gettimeofday(start, &tzp);
      return 0;
    } else {
      timeval end;
      long ms = 0;
      gettimeofday(&end, &tzp);
      ms = (end.tv_sec - start->tv_sec) * 1000;
      ms += ((int) end.tv_usec - (int) start->tv_usec) / 1000;
      return (int) ms;
    }
    #endif
}


extern "C++"
inline
unsigned int example_random(unsigned int n)
{
 return (((unsigned int) 1103515245 * n) + (unsigned int) 12345) %
        (unsigned int) 0xFFFFFFFF;
}
#endif 
