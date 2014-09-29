#include "Drivers/Clock.h"

uint8_t* const Clock::PCMClockBaseAddress = (uint8_t* const)  0x20101098;
uint8_t* const Clock::PWMClockBaseAddress = (uint8_t* const)  0x201010A0;

Clock::Clock(uint8_t* baseAddress):
   baseAddress(baseAddress)
{}

Clock::~Clock(){}

void Clock::setClockSource(ClockSource source)
{
   writeCLKCTL_SRC(baseAddress, source);
}

void Clock::kill()
{
   writeCLKCTL_KILL(baseAddress, 1);
}

void Clock::enable()
{
   writeCLKCTL_ENAB(baseAddress, 1);
}

void Clock::disable()
{
   writeCLKCTL_ENAB(baseAddress, 0);
}

bool Clock::isBusy()
{
   return readCLKCTL_BUSY(baseAddress);
}

void Clock::invertClock(bool invert)
{
   writeCLKCTL_FLIP(baseAddress, invert);
}


void Clock::setMASHControl(MASH mash)
{
   writeCLKCTL_MASH(baseAddress, mash);
}

void Clock::setDivisor(uint16_t integer, uint16_t fraction)
{
   writeCLKDIV(baseAddress, (integer << 12) | fraction);
}
