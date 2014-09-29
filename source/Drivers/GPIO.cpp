#include <Drivers/GPIO.h>
#include <cstddef>

static void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
	     : : [count]"r"(count) : "cc");
}

GPIO& GPIO::getSingleton()
{
   static GPIO gpio(reinterpret_cast<uint8_t*>(0x20200000));

   return gpio;
}

GPIO::GPIO(uint8_t* baseAddress):
   baseAddress(baseAddress)
{}

void GPIO::setPinFunction(uint8_t pin, PinFunction function)
{
   uint32_t pinOffset = (pin%10)*3;
   uint32_t registerValue = 0;
   RegReadFunctionPtr readRegister = NULL;
   RegWriteFunctionPtr writeRegister = NULL;
   if(pin < 10)
   {
      readRegister = readGPFSEL0;
      writeRegister = writeGPFSEL0;
   }
   else if (pin < 20)
   {
      readRegister = readGPFSEL1;
      writeRegister = writeGPFSEL1;
   }
   else if (pin < 30)
   {
      readRegister = readGPFSEL2;
      writeRegister = writeGPFSEL2;
   }
   else if (pin < 40)
   {
      readRegister = readGPFSEL3;
      writeRegister = writeGPFSEL3;
   }
   else if (pin < 50)
   {
      readRegister = readGPFSEL4;
      writeRegister = writeGPFSEL4;
   }
   else if (pin < 54)
   {
      readRegister = readGPFSEL5;
      writeRegister = writeGPFSEL5;
   }

   if(readRegister && writeRegister)
   {
      registerValue = readRegister(baseAddress);
      registerValue &= ~(0x7 << pinOffset);
      registerValue |= (function << pinOffset);
      writeRegister(baseAddress, registerValue);
   }
}

void GPIO::setPinLevel(uint8_t pin, PinLevel level)
{
   if(pin < 32)
   {
      if(level == pinLevelHigh)
      {
         writeGPSET0(baseAddress, getPinMask(pin));
      }
      else
      {
         writeGPCLR0(baseAddress, getPinMask(pin));
      }
   }
   else if (pin < 64)
   {
      if(level == pinLevelHigh)
      {
         writeGPSET1(baseAddress, getPinMask(pin));
      }
      else
      {
         writeGPCLR1(baseAddress, getPinMask(pin));
      }
   }
}

GPIO::PinLevel GPIO::getPinLevel(uint8_t pin)
{
   if(pin < 32)
   {
      return static_cast<PinLevel>((readGPLEV0(baseAddress) >> pin) & 1);
   }
   else if (pin < 64)
   {
      return static_cast<PinLevel>((readGPLEV1(baseAddress) >> (pin%32)) & 1);
   }
   return pinLevelLow;
}

bool GPIO::eventDetected(uint8_t pin)
{
   if(pin < 32)
   {
      return (readGPEDS0(baseAddress) >> pin) & 1;
   }
   else if (pin < 64)
   {
      return (readGPEDS1(baseAddress) >> (pin%32)) & 1;
   }
   return false;
}

void GPIO::enablePinDetect(uint8_t pin, PinDetectType pinDetectType, EdgeDetectTime edgeTime)
{
   if((pinDetectType == pinDetectTypeHigh) || (pinDetectType == pinDetectTypeLow))
   {
      setLevelDetect(pin, pinDetectType);
      setAsynchronousPinTrasitionDetect(pin, pinDetectTypeNone);
      setSynchronousPinTrasitionDetect(pin, pinDetectTypeNone);
   }
   else if(edgeTime == edgeDetectTimeSynchronous)
   {
      setLevelDetect(pin, pinDetectTypeNone);
      setAsynchronousPinTrasitionDetect(pin, pinDetectTypeNone);
      setSynchronousPinTrasitionDetect(pin, pinDetectType);
   }
   else if(edgeTime == edgeDetectTimeAsynchronous)
   {
      setLevelDetect(pin, pinDetectTypeNone);
      setSynchronousPinTrasitionDetect(pin, pinDetectTypeNone);
      setAsynchronousPinTrasitionDetect(pin, pinDetectType);
   }
}

void GPIO::setSynchronousPinTrasitionDetect(uint8_t pin, PinDetectType pinDetectType)
{
   RegReadFunctionPtr readRisingEdgeRegister = NULL;
   RegWriteFunctionPtr writeRisingEdgeRegister = NULL;
   RegReadFunctionPtr readFallingEdgeRegister = NULL;
   RegWriteFunctionPtr writeFallingEdgeRegister = NULL;
   uint32_t registerValue;

   if(pin < 32)
   {
      readRisingEdgeRegister = readGPREN0;
      writeRisingEdgeRegister = writeGPREN0;
      readFallingEdgeRegister = readGPFEN0;
      writeFallingEdgeRegister = writeGPFEN0;
   }
   else if (pin < 64)
   {
      readRisingEdgeRegister = readGPREN1;
      writeRisingEdgeRegister = writeGPREN1;
      readFallingEdgeRegister = readGPFEN1;
      writeFallingEdgeRegister = writeGPFEN1;
   }

   if(pinDetectType == pinDetectTypeNone)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeRising)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeFalling)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeRisingAndFalling)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
}

void GPIO::setAsynchronousPinTrasitionDetect(uint8_t pin, PinDetectType pinDetectType)
{
   RegReadFunctionPtr readRisingEdgeRegister = NULL;
   RegWriteFunctionPtr writeRisingEdgeRegister = NULL;
   RegReadFunctionPtr readFallingEdgeRegister = NULL;
   RegWriteFunctionPtr writeFallingEdgeRegister = NULL;
   uint32_t registerValue;

   if(pin < 32)
   {
      readRisingEdgeRegister = readGPAREN0;
      writeRisingEdgeRegister = writeGPAREN0;
      readFallingEdgeRegister = readGPAFEN0;
      writeFallingEdgeRegister = writeGPAFEN0;
   }
   else if (pin < 64)
   {
      readRisingEdgeRegister = readGPAREN1;
      writeRisingEdgeRegister = writeGPAREN1;
      readFallingEdgeRegister = readGPAFEN1;
      writeFallingEdgeRegister = writeGPAFEN1;
   }

   if(pinDetectType == pinDetectTypeNone)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeRising)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeFalling)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
   else if(pinDetectType == pinDetectTypeRisingAndFalling)
   {
      registerValue = readRisingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeRisingEdgeRegister(baseAddress, registerValue);

      registerValue = readFallingEdgeRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeFallingEdgeRegister(baseAddress, registerValue);
   }
}

void GPIO::setLevelDetect(uint8_t pin, PinDetectType level)
{
   RegReadFunctionPtr readHighLevelRegister = NULL;
   RegWriteFunctionPtr writeHighLevelRegister = NULL;
   RegReadFunctionPtr readLowLevelRegister = NULL;
   RegWriteFunctionPtr writeLowLevelRegister = NULL;
   uint32_t registerValue;

   if(pin < 32)
   {
      readHighLevelRegister = readGPHEN0;
      writeHighLevelRegister = writeGPHEN0;
      readLowLevelRegister = readGPLEN0;
      writeLowLevelRegister = writeGPLEN0;
   }
   else if (pin < 64)
   {
      readHighLevelRegister = readGPHEN1;
      writeHighLevelRegister = writeGPHEN1;
      readLowLevelRegister = readGPHEN1;
      writeLowLevelRegister = writeGPLEN1;
   }

   if(level == pinDetectTypeNone)
   {
      registerValue = readHighLevelRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeHighLevelRegister(baseAddress, registerValue);

      registerValue = readLowLevelRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeLowLevelRegister(baseAddress, registerValue);
   }
   else if(level == pinDetectTypeHigh)
   {
      registerValue = readHighLevelRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeHighLevelRegister(baseAddress, registerValue);

      registerValue = readLowLevelRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeLowLevelRegister(baseAddress, registerValue);
   }
   else if(level == pinDetectTypeLow)
   {
      registerValue = readHighLevelRegister(baseAddress);
      registerValue &= ~(1<<getPinMask(pin));
      writeHighLevelRegister(baseAddress, registerValue);

      registerValue = readLowLevelRegister(baseAddress);
      registerValue |= 1<<getPinMask(pin);
      writeLowLevelRegister(baseAddress, registerValue);
   }
}

void GPIO::setPinPullDirection(uint8_t pin, PullDirection direction)
{
   writeGPPUD(baseAddress, direction);
   delay(150);
   if(pin < 32)
   {
      writeGPPUDCLK0(baseAddress, getPinMask(pin));
   }
   else if (pin < 64)
   {
      writeGPPUDCLK1(baseAddress, getPinMask(pin));
   }
   delay(150);
   writeGPPUD(baseAddress, 0);
   writeGPPUDCLK0(baseAddress, 0);
   writeGPPUDCLK1(baseAddress, 0);
}

uint32_t GPIO::getPinMask(uint8_t pin)
{
   return 1<<(pin%32);
}