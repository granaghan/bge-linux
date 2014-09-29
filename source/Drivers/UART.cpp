#include "Drivers/UART.h"
#include "codegen/UART.h"
#include "Drivers/GPIO.h"
#include "constants.h"
#include <stdint.h>

uint8_t* const UART::UART0BaseAddress = reinterpret_cast<uint8_t*>(0x20201000);

UART::UART(uint8_t* baseAddress):
   baseAddress(baseAddress)
{
   writeCR(baseAddress, 0);
   writeCR_UARTEN(baseAddress, 1);
   writeCR_TXE(baseAddress, 1);
   writeCR_RXE(baseAddress, 1);
   writeLCRH_FEN(baseAddress, 1);
}

UART::~UART()
{}

void UART::setBaudRate(uint32_t baudRate)
{
   uint32_t rate = (3000000 * 4) / baudRate;
   writeIBRD_IBRD(baseAddress, rate >> 6);
   writeFBRD_FBRD(baseAddress, rate & ((1<<6)-1));
}

void UART::setWordSize(uint32_t sizeInBits)
{
   switch(sizeInBits)
   {
      case 5:
         writeLCRH_WLEN(baseAddress, 0);
         break;
      case 6:
         writeLCRH_WLEN(baseAddress, 1);
         break;
      case 7:
         writeLCRH_WLEN(baseAddress, 2);
         break;
      case 8:
         writeLCRH_WLEN(baseAddress, 3);
         break;
      default:
         // error here
         break;
   }
}

void UART::setParity(Parity parity)
{
   switch(parity)
   {
      case parityNone:
         writeLCRH_PEN(baseAddress, 0);
         break;
      case parityEven:
         writeLCRH_EPS(baseAddress, 1);
         writeLCRH_PEN(baseAddress, 1);
         break;
      case parityOdd:
         writeLCRH_EPS(baseAddress, 2);
         writeLCRH_PEN(baseAddress, 1);
         break;
   }
}

void UART::setStopBitCount(uint32_t stopBitCount)
{
   writeLCRH_STP2(baseAddress, stopBitCount==2);
}

void UART::enableTransmit(uint8_t enable)
{
   writeCR_TXE(baseAddress, enable);
}

void UART::enableReceive(uint8_t enable)
{
   writeCR_RXE(baseAddress, enable);
}

void UART::sendByte(uint8_t data)
{
   GPIO& gpio = GPIO::getSingleton();
   uint32_t i = 0;
   while(readFR_TXFF(baseAddress));
   writeDR_Data(baseAddress, data);
}

bool UART::txFIFOFull()
{
   return readFR_TXFF(baseAddress);
}

bool UART::txFIFOEmpty()
{
   return readFR_TXFE(baseAddress);
}
