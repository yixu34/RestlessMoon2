#ifndef TIMER_HPP
#define TIMER_HPP

//
// Miscellaneous timer utility functions.
//

void updateTime();
float getTimeSeconds();
float getTimeMilliseconds();
float getDtMilliseconds();
float getOldDtMilliseconds();
float getDtSeconds();
float getOldDtSeconds();
float getFrameRate();

#endif