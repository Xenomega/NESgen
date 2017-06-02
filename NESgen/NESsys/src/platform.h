#ifndef PLATFORM_H_
#define PLATFORM_H_

// Generic includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include <time.h>

// ---------------------------------
// OS Specific Includes
// ---------------------------------
#ifndef GL_UNSIGNED_INT_8_8_8_8
// Header files on Windows are out of date and often don't include this. Needed to specify framebuffer format.
#define GL_UNSIGNED_INT_8_8_8_8			0x8035
#endif

#ifdef _WIN32
	// WINDOWS
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include <gl/glut.h>
	#include <gl/freeglut.h>
#else
	// Generic UNIX
	#include "unistd.h"
	#include "pthread.h"

	// MacOS/Linux Specific
	#if __APPLE__
		// APPLE DEVICES
    	#include "TargetConditionals.h"
		// Only tested support for Mac OS (not iPhone/iPhone Simulator)
    	#if TARGET_OS_MAC
			#include "GLUT/glut.h"
			extern void glutWMCloseFunc(void (* function)(void));
			#define glutCloseFunc(x)		glutWMCloseFunc(x)
    	#endif
	#else
			// LINUX / Other
    	#include "GL/glut.h"
			extern void glutCloseFunc(void (* function)(void)); // does not resolve with some versions of glut on linux.
	#endif
#endif

// ---------------------------------
// OS Specific Definitions
// ---------------------------------
#ifdef _WIN32
	typedef HANDLE THREADHANDLE;
	typedef LARGE_INTEGER TIMEDATA;
#else
	typedef pthread_t THREADHANDLE;
	typedef struct timespec TIMEDATA;
	typedef unsigned char BOOL;
	#define TRUE	1
	#define FALSE	0
	typedef char CHAR;
	typedef unsigned char BYTE;
	typedef short SHORT;
	typedef unsigned short USHORT;
	typedef int INT;
	typedef unsigned int UINT;
	typedef long LONG;
	typedef unsigned long ULONG;
	typedef long long LONGLONG;
	typedef unsigned long long ULONGLONG;
	typedef float FLOAT;
	typedef double DOUBLE;
	#define min(a,b)	(a <= b ? a : b)
	#define max(a,b)	(a >= b ? a : b)
#endif


// ---------------------------------
// Functions
// ---------------------------------
void console_log(const char *fmt, ...);
void debug_log(const char *fmt, ...);
void error(const char *fmt, ...);
THREADHANDLE create_thread(void (* function)(void));
void destroy_thread(THREADHANDLE handle);
void get_time(TIMEDATA* time);
ULONGLONG get_time_difference(TIMEDATA* start, TIMEDATA* end);
void thread_sleep(UINT milli);

#endif /* PLATFORM_H_ */
