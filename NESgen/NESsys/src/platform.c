#include "NESsys.h"
#include "platform.h"

/*
 * Prints out the given information, keeping in mind the updated console information meant to stick at the bottom.
 */
void console_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
/*
 * Prints out the given information if debug logging is enabled, keeping in mind the updated console information meant to stick at the bottom.
 */
void debug_log(const char *fmt, ...)
{
#if APPLICATION_DEBUGLOG
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}

/*
 * Throws the error with the given format and exits the application.
 */
void error(const char *fmt, ...)
{
	printf("ERROR: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

/*
 * Creates a thread executing the given function.
 */
THREADHANDLE create_thread(void (* function)(void))
{
#ifdef _WIN32
	return CreateThread(NULL, 0, (void*)function, NULL, 0, NULL);
#else
	// Create a thread with the given function
	pthread_t handle;
	int err = pthread_create(&handle, NULL, (void*)function, NULL);
	if(err != 0)
		error("Can't create game thread.");
	return handle;
#endif
}
/*
 * Destroys/Terminates a given thread.
 */
void destroy_thread(THREADHANDLE handle)
{
#ifdef _WIN32
	TerminateThread(handle, 0);
	CloseHandle(handle);
#else
	pthread_cancel(handle);
#endif
}
/*
 * Obtains the current time.
 */
void get_time(TIMEDATA* time)
{
#ifdef _WIN32
	QueryPerformanceCounter(time);
#else
	clock_gettime(CLOCK_MONOTONIC, time);
#endif
}
/*
 * Obtains an absolute time difference in milliseconds.
 */
ULONGLONG get_time_difference(TIMEDATA* start, TIMEDATA* end)
{
	// Source: http://stackoverflow.com/questions/17639213/millisecond-precision-timing-of-functions-in-c-crossplatform
#ifdef _WIN32
    LARGE_INTEGER Frequency, elapsed;

    QueryPerformanceFrequency(&Frequency);
    if(end->QuadPart > start->QuadPart)
    	elapsed.QuadPart = end->QuadPart - start->QuadPart;
    else
    	elapsed.QuadPart = start->QuadPart - end->QuadPart;

    elapsed.QuadPart *= 1000;
    elapsed.QuadPart /= Frequency.QuadPart;

    return (ULONGLONG)elapsed.QuadPart;
#else
    ULONGLONG endMilli = ((end->tv_sec * 1000) + (end->tv_nsec / 1000000));
    ULONGLONG startMilli = ((start->tv_sec * 1000) + (start->tv_nsec / 1000000));
    if(endMilli > startMilli)
    	return endMilli - startMilli;
    else
    	return startMilli - endMilli;
#endif
}

/*
 * Sleeps for a given time (in milliseconds).
 */
void thread_sleep(UINT milli)
{
#ifdef _WIN32
	Sleep(milli);
#else
	usleep(milli * 1000);
#endif
}
