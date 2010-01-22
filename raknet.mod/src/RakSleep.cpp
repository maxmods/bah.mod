#if defined(_XBOX) || defined(X360)
                            
#elif defined(_WIN32)
#include "WindowsIncludes.h" // Sleep
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
                                                
#else
#include <unistd.h> // usleep
#include <time.h>
#endif


#include "RakSleep.h"

void RakSleep(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
                                                                                            
#else

	timespec ts;

	ts.tv_sec=0;

	ts.tv_nsec=ms*1000*1000;
	// usleep(ms * 1000);
	nanosleep(&ts,0);
#endif
}
