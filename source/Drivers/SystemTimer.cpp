#include "Drivers/SystemTimer.h"

uint8_t* const SystemTimer::SystemTimerBaseAddress = (uint8_t* const) 0x20003000;
const uint32_t SystemTimer::SystemTimerTickRateInHz = 1000000;

SystemTimer::SystemTimer(uint8_t* baseAddress):
   baseAddress(baseAddress)
{}

SystemTimer::~SystemTimer(){}

uint64_t SystemTimer::getTimeInTicks()
{
   uint32_t high = readCHI_CNT(baseAddress);
   uint32_t low = readCLO_CNT(baseAddress);
   uint32_t newHigh = readCHI_CNT(baseAddress);
   if(newHigh != high)
   {
      high = newHigh;
      low = 0;
   }

   return ( (static_cast<uint64_t>(high) << 32U) | low);
}

uint64_t SystemTimer::getTimeInSeconds()
{
   return (getTimeInTicks() / SystemTimerTickRateInHz);
}

void SystemTimer::setMatchValue(uint8_t channel, uint32_t matchValue)
{
   switch(channel)
   {
      case 0:
         writeC0_CMP(baseAddress, matchValue);
      case 1:
         writeC1_CMP(baseAddress, matchValue);
      case 2:
         writeC2_CMP(baseAddress, matchValue);
      case 3:
         writeC3_CMP(baseAddress, matchValue);
   }
}

bool SystemTimer::checkForMatch(uint8_t channel)
{
   switch(channel)
   {
      case 0:
         return readSYSTIMER_CS_M0(baseAddress) != 0;
      case 1:
         return readSYSTIMER_CS_M1(baseAddress) != 0;
      case 2:
         return readSYSTIMER_CS_M2(baseAddress) != 0;
      case 3:
         return readSYSTIMER_CS_M3(baseAddress) != 0;
   }
   return false;
}