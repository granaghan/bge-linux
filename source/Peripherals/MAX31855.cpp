#include "Peripherals/MAX31855.h"
#include <stdio.h>

MAX31855::MAX31855(SPI& spi, uint8_t chipSelectLine):
   spi(spi),
   chipSelectLine(chipSelectLine)
{
   spi.setChannelChipSelectPolarity(chipSelectLine, SPI::polarityLow);
   spi.setTransferActive(true);
}

MAX31855::~MAX31855(){}

uint32_t MAX31855::readTemperature()
{
   uint32_t data = 0;

   spi.assertChipSelect(chipSelectLine);
   spi.writeData(0xAA);
   spi.writeData(0xAA);
   spi.writeData(0xAA);
   spi.writeData(0xAA);

   while(!spi.dataAvailable());
   data = (data << 8) | spi.readData();

   while(!spi.dataAvailable());
   data = (data << 8) | spi.readData();

   while(!spi.dataAvailable());
   data = (data << 8) | spi.readData();

   while(!spi.dataAvailable());

   data = (data << 8) | spi.readData();
   data = data >> 20;
   if(data & (1<<11))
   {
      data |= ~(0x7FF);
   }
   
   spi.assertChipSelect(3);
   return data;

   return 0;
}
