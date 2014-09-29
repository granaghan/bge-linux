#ifndef DRIVERS_SYSTEMTIMER_H_
#define DRIVERS_SYSTEMTIMER_H_

#include <codegen/SYSTIMER.h>
#include <FreeRTOSConfig.h>
#include <stdint.h>

class SystemTimer
{
   public:
      SystemTimer(uint8_t* baseAddress);
      ~SystemTimer();

      uint64_t getTimeInTicks();
      uint64_t getTimeInSeconds();
      void setMatchValue(uint8_t channel, uint32_t matchValue);
      bool checkForMatch(uint8_t channel);

      static uint8_t* const SystemTimerBaseAddress;
      static const uint32_t SystemTimerTickRateInHz;

   private:
      uint8_t* baseAddress;
};

#endif
