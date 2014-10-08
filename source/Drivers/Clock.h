#ifndef DRIVERS_CLOCK_H_
#define DRIVERS_CLOCK_H_

#include <codegen/CLOCK.h>
#include <stdint.h>

class Clock
{
   public:
      typedef enum
      {
         clockSourceGround,
         clockSourceOscillator,
         clockSourceTestDebug0,
         clockSourceTestDebug1,
         clockSourcePLLA,
         clockSourcePLLC,
         clockSourcePLLD,
         clockSourceHDMIAux
      } ClockSource;

      typedef enum
      {
         MASHNone,
         MASH1Stage,
         MASH2Stage,
         MASH3Stage
      } MASH;

      Clock(uint8_t* baseAddress);

      ~Clock();

      void setClockSource(ClockSource source);
      void kill();
      void enable();
      void disable();
      bool isBusy();
      void invertClock(bool invert);
      void setMASHControl(MASH mash);

      void setDivisor(uint16_t integer, uint16_t fraction);
      
      static uint8_t* const PCMClockBaseAddress;
      static uint8_t* const PWMClockBaseAddress;

   private:
      uint8_t* baseAddress;
};

#endif
