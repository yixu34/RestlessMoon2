#ifndef LOG_HPP
#define LOG_HPP

#include <string>

//
// File logging system.
//
void initLog(const std::string &logFilename);
void log(const char *logText, ...);
void shutdownLog();

#endif