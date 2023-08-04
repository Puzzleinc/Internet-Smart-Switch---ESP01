#ifndef timefunction_h

#define timefunction_h
#include <Arduino.h>

// the #include statement and code go here...
void clockCounter(unsigned long currentMillis, unsigned long &previousMillis, uint8_t &seconds, uint8_t &minutes, uint8_t &hours, uint16_t &days);
void showTime(unsigned long currentMillis, unsigned long &timeLast , uint8_t const intervalSec, uint8_t secIndicator);

#endif