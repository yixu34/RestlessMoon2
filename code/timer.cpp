//#include <ctime>
#include <windows.h>

#include "timer.hpp"

namespace
{
	float currentTimeSeconds      = 0.0f;
	float currentTimeMilliseconds = 0.0f;
	float timeDelta               = 0.0f;
	float oldTimeDelta            = 0.0f;

	float currentFrameRate  = 0.0f;
	float returnedFrameRate = 0.0f;
	float currentFrameTime  = 0.0f;
}

struct 
{
	__int64       frequency;							
	float         resolution;							
	unsigned long mm_timer_start;						
	unsigned long mm_timer_elapsed;
	bool          performance_timer;
	__int64       performance_timer_start;
	__int64       performance_timer_elapsed;		
} timer;

void initTimer()
{
	memset(&timer, 0, sizeof(timer));					// Clear Our Timer Structure

	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &timer.frequency))
	{
		// No Performace Counter Available
		timer.performance_timer	= FALSE;				// Set Performance Timer To FALSE
		timer.mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		timer.resolution	= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		timer.frequency		= 1000;					// Set Our Timer Frequency To 1000
		timer.mm_timer_elapsed	= timer.mm_timer_start;			// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &timer.performance_timer_start);
		timer.performance_timer		= TRUE;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		timer.resolution		= (float) (((double)1.0f)/((double)timer.frequency));
		// Set The Elapsed Time To The Current Time
		timer.performance_timer_elapsed	= timer.performance_timer_start;
	}
}

float getTimeNow()								// Get Time In Milliseconds
{
	__int64 time;								// time Will Hold A 64 Bit Integer

	if (timer.performance_timer)						// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);		// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - timer.performance_timer_start) * timer.resolution)*1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - timer.mm_timer_start) * timer.resolution)*1000.0f;
	}
}

void updateFrameRate();

void updateTime()
{
	static bool initializedTime = false;
	if (!initializedTime)
	{
		initTimer();
		initializedTime = true;
	}

	float prevTime          = currentTimeMilliseconds;
	//currentTimeMilliseconds = static_cast<float>(std::clock());
	currentTimeMilliseconds = getTimeNow();
	currentTimeSeconds      = currentTimeMilliseconds / 1000.0f;
	oldTimeDelta = timeDelta;

	if (prevTime != 0.0f) // if not the first frame..
	{
		if (currentTimeMilliseconds >= prevTime)  // .. and the time increased
			timeDelta = currentTimeMilliseconds - prevTime; // then set the timeDelta.
		else
			timeDelta = 0.0f;  // else fall back to 0 (due to wrap around)
	}

	updateFrameRate();
}

float getTimeSeconds()
{
	return currentTimeSeconds;
}

float getTimeMilliseconds()
{
	return currentTimeMilliseconds;
}

float getDtMilliseconds()
{
	return timeDelta;
}

float getOldDtMilliseconds()
{
	return oldTimeDelta;
}

float getDtSeconds()
{
	return timeDelta / 1000.0f;
}

float getOldDtSeconds()
{
	return oldTimeDelta / 1000.0f;
}

float getFrameRate()
{
	return returnedFrameRate;
}

void updateFrameRate()
{
	//if (static_cast<float>(std::clock()) - currentFrameTime >= 1000.0f)
	float timeNow = getTimeNow();
	if (timeNow - currentFrameTime >= 1000.0f)
	{
		returnedFrameRate = currentFrameRate;
		currentFrameRate  = 0;
		currentFrameTime  = timeNow;
	}
	else
		currentFrameRate++;
}
