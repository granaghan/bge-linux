#include "Peripherals/SparkfunLCD.h"
#include "Drivers/UART.h"
#include <stdint.h>


SparkfunLCD::SparkfunLCD(UART& uart):
   uart(uart)
{
   uart.setBaudRate(9600);
   uart.setWordSize(8);
   uart.setParity(UART::parityNone);
   uart.setStopBitCount(1);
   uart.enableTransmit(true);
}

SparkfunLCD::~SparkfunLCD(){}

void SparkfunLCD::setBacklightBrightness(uint8_t level)
{
   uint32_t scaledLevel = (static_cast<uint32_t>(level)*29)/100+128;
   uart.sendByte(altCommandChar);
   uart.sendByte(scaledLevel);
   // Sending more than two bytes to the UART too soon seems
   // to hang the chip, so delay here. :(
}

void SparkfunLCD::clear()
{
   uart.sendByte(commandChar);
   uart.sendByte(clearCommand);
}

void SparkfunLCD::cursorLeft()
{
   uart.sendByte(commandChar);
   uart.sendByte(moveCursorLeftCommand);
}

void SparkfunLCD::cursorRight()
{
   uart.sendByte(commandChar);
   uart.sendByte(moveCursorRightCommand);
}

void SparkfunLCD::setCursorPosition(uint8_t position)
{
   uart.sendByte(commandChar);
   uart.sendByte(setCursorCommand + (position & 0x7F));
}

void SparkfunLCD::scrollLeft()
{
   uart.sendByte(commandChar);
   uart.sendByte(clearCommand);
}

void SparkfunLCD::scrollRight()
{
   uart.sendByte(commandChar);
   uart.sendByte(clearCommand);
}

void SparkfunLCD::enableCursorUnderline(bool enable)
{
   uart.sendByte(commandChar);
   if(enable)
   {
      uart.sendByte(underlineOnCommand);
   }
   else
   {
      uart.sendByte(underlineOffCommand);
   }
}

void SparkfunLCD::enableCursorBlink(bool enable)
{
   uart.sendByte(commandChar);
   if(enable)
   {
      uart.sendByte(boxOnCommand);
   }
   else
   {
      uart.sendByte(boxOffCommand);
   }
}

void SparkfunLCD::sendCharacter(char c)
{
   uart.sendByte(c);
}

void SparkfunLCD::sendString(const char* str)
{
   while(*str)
   {
      sendCharacter(*str++);
   }
}
