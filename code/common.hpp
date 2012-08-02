#ifndef COMMON_HPP
#define COMMON_HPP

// 
// Compiler options and other useful global data.
// 

// Common includes.
#include "log.hpp"
#include <cassert>
#include <cctype>

// Macros.
#ifdef _DEBUG
#	define MYASSERT(exp)      \
		do {                  \
			if(!(exp))	{      \
				shutdownLog(); \
				assert(exp);     \
			}                  \
		} while(0)

#	define ASSERTMSG(exp, msg) \
		do {                     \
			if (!(exp)) {        \
				log(msg);        \
				shutdownLog();    \
				assert(exp && msg);        \
			}                     \
		} while (0)

#	define ERRORMSG(msg) \
		do {			\
			log(msg);	\
			shutdownLog(); \
			assert(false && msg); \
		} while(0)
#else
#	define MYASSERT(exp) do {} while(0)
#	define EXIT_ASSERT do {} while(0)
#	define ASSERTMSG(exp, msg) do {} while(0)
#	define ERRORMSG(msg) \
	do {	\
		log(msg);	\
	} while (0)
#endif


// Visual Studio compiler options.
#ifdef _MSC_VER
	#define _USE_MATH_DEFINES
	#define vsnprintf _vsnprintf
	#define strcasecmp _stricmp

	#pragma warning (disable: 4065)  // switch statement contains 'default' but no 'case' labels
	#pragma warning (disable: 4244)  // conversion from __w64 int to int
	#pragma warning (disable: 4267)  // conversion from size_t to int
	#pragma warning (disable: 4018)
	#pragma warning (disable: 4305)  // truncation from 'double' to 'float'
	#pragma warning (disable: 4800)  // forcing value to bool 'true' or 'false' (performance warning)
#endif

#endif