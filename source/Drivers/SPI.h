#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

#include <codegen/SPI.h>
#include <stdint.h>

class SPI
{
   public:
      typedef enum
      {
         polarityLow,
         polarityHigh
      } Polarity;

      SPI(uint8_t* baseAddress);
      ~SPI();

      void setClockRate(uint32_t frequency);
      void setClockPolarity(Polarity polarity);
      void setChannelChipSelectPolarity(uint32_t channel, Polarity polarity);
      void setChipSelectPolarity(Polarity polarity);
      bool dataAvailable();
      bool transmitFIFOFull();
      void writeData(uint8_t data);
      uint8_t readData();
      void assertChipSelect(uint8_t channel);
      void setTransferActive(bool active);

      static uint8_t* const SPI0BaseAddress;

   private:
      static const uint32_t coreClockRate;
      uint8_t* baseAddress;
};

#endif
