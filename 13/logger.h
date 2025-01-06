#ifndef LOGGER_H_LOADED
#define LOGGER_H_LOADED

#include <stdio.h>
#include <unistd.h>

#ifdef LOG_I
	#define LOG_W
#endif

#ifdef LOG_W
	#define LW(x) L("WARN", x)
#else
	#define LW(x)
#endif

#ifdef LOG_I
	#define LI(x) L("INFO", x)
#else
	#define LI(x)
#endif

#ifdef LOG_ON
	#define L(lvl, x) fprintf(stderr, "[%s] PID:%d %s:%d::%s %s\n", __TIME__, getpid(), __FILE__, __LINE__, __FUNCTION__, x);
#else
	#define L(lvl, x)
#endif

#endif