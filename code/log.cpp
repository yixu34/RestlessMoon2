#include "common.hpp"
#include <cstdio>
#include <cstdarg>

std::FILE *logFile  = 0;
bool isReady        = false;
bool isShuttingDown = false;

void initLog(const std::string &logFilename)
{
	if (logFile != 0)
	{
		// Ignore the request if the log file is already open.
		return;
		//fclose(logFile);
		//logFile = 0;
	}

	logFile = fopen(logFilename.c_str(), "wt+");
	if (logFile == 0)
		ASSERTMSG(false, "Couldn't initialize log.");

	isReady = true;
}

void log(const char *logText, ...)
{
	assert(isReady);

	//const char *logText = text.c_str();

	va_list varArgs;
	va_start(varArgs, logText);

	static char buf[1024];
	vsnprintf(buf, 1024, logText, varArgs);
	va_end(varArgs);

	strcat(buf, "\n");

	fprintf(logFile, buf);
}

void shutdownLog()
{
	if (!isShuttingDown)
	{
		if (logFile != 0)
			fclose(logFile);

		logFile = 0;
		isReady = false;

		isShuttingDown = true;
	}
}
