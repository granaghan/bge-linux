#ifndef PERIPHERALS_MAX31855_H_
#define PERIPHERALS_MAX31855_H_
#include "Drivers/SPI.h"
#include <stdint.h>

class MAX31855
{
   public:
      MAX31855(SPI& spi, uint8_t chipSelectLine);
      ~MAX31855();

      uint32_t readTemperature();

   private:
      SPI& spi;
      uint8_t chipSelectLine;
};

#endif