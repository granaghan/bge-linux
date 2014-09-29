#include "Drivers/SPI.h"
#include <codegen/SPI.h>
#include <FreeRTOSConfig.h>
#include <stdint.h>

uint8_t* const SPI::SPI0BaseAddress = (uint8_t* const)0x20204000;
const uint32_t SPI::coreClockRate = 250000000;

typedef enum
{
   polarityLow,
   polarityHigh
} Polarity;

SPI::SPI(uint8_t* baseAddress):
   baseAddress(baseAddress)
{
   writeCS_CPHA(baseAddress, 1);
   writeCS_REN(baseAddress, 1);
   writeCS_LEN_LONG(baseAddress, 0);
}

SPI::~SPI(){}

void SPI::setClockRate(uint32_t frequency)
{
   uint32_t divider = coreClockRate/frequency;
   writeCLK_ClockDivider(baseAddress, divider);
}

void SPI::setClockPolarity(Polarity polarity)
{
   writeCS_CPOL(baseAddress, polarity);
}

void SPI::setChannelChipSelectPolarity(uint32_t channel, Polarity polarity)
{
   switch(channel)
   {
      case 0:
         writeCS_CSPOL0(baseAddress, polarity);
         break;
      case 1:
         writeCS_CSPOL1(baseAddress, polarity);
         break;
      case 2:
         writeCS_CSPOL2(baseAddress, polarity);
         break;
   }
}

void SPI::setChipSelectPolarity(Polarity polarity)
{
   writeCS_CSPOL(baseAddress, polarity);
}

bool SPI::dataAvailable()
{
   return readCS_RXD(baseAddress);
}

bool SPI::transmitFIFOFull()
{
   return !readCS_TXD(baseAddress);
}

void SPI::writeData(uint8_t data)
{
   writeFIFO(baseAddress, data);
}

uint8_t SPI::readData()
{
   return readFIFO(baseAddress);
}

void SPI::assertChipSelect(uint8_t channel)
{
   writeCS_CS(baseAddress, channel);
}

void SPI::setTransferActive(bool active)
{
   writeCS_TA(baseAddress, active);
}
