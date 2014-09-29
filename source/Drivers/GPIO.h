#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <codegen/GPIO.h>
#include <FreeRTOSConfig.h>
#include <stdint.h>

class GPIO
{
   public:
      typedef enum
      {
         pinFunctionInput,
         pinFunctionOutput,
         pinFunctionAlternate5,
         pinFunctionAlternate4,
         pinFunctionAlternate0,
         pinFunctionAlternate1,
         pinFunctionAlternate2,
         pinFunctionAlternate3
      } PinFunction;

      typedef enum
      {
         pinLevelLow,
         pinLevelHigh
      } PinLevel;

      typedef enum
      {
         edgeDetectLevelHigh,
         edgeDetectLevelLow
      } EdgeDetectLevel;

      typedef enum
      {
         edgeDetectTimeSynchronous,
         edgeDetectTimeAsynchronous
      } EdgeDetectTime;

      typedef enum
      {
         pullDirectionNone,
         pullDirectionUp,
         pullDirectionDown
      } PullDirection;

      typedef enum
      {
         pinDetectTypeNone,
         pinDetectTypeRising,
         pinDetectTypeFalling,
         pinDetectTypeRisingAndFalling,
         pinDetectTypeHigh,
         pinDetectTypeLow
      } PinDetectType;

      static GPIO& getSingleton();

      ~GPIO(){}

      void setPinFunction(uint8_t pin, PinFunction function);

      void setPinLevel(uint8_t pin, PinLevel level);

      PinLevel getPinLevel(uint8_t pin);

      bool eventDetected(uint8_t pin);

      void enablePinDetect(uint8_t pin, PinDetectType PinDetectType, EdgeDetectTime edgeTime);

      void setPinPullDirection(uint8_t pin, PullDirection direction);

   private:
      GPIO(){}
      GPIO(uint8_t* baseAddress);
      GPIO(GPIO const&);
      void operator=(GPIO const&);

      typedef uint32_t(*RegReadFunctionPtr)(uint8_t*);
      typedef void(*RegWriteFunctionPtr)(uint8_t*,uint32_t);

      void setLevelDetect(uint8_t pin, PinDetectType level);
      void setSynchronousPinTrasitionDetect(uint8_t pin, PinDetectType edge);
      void setAsynchronousPinTrasitionDetect(uint8_t pin, PinDetectType edge);
      uint32_t getPinMask(uint8_t pin);

      uint8_t* baseAddress;
};

#endif
