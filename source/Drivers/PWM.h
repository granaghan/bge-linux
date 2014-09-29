#ifndef DRIVERS_PWM_H_
#define DRIVERS_PWM_H_

#include <codegen/PWM.h>
#include <FreeRTOSConfig.h>
#include <stdint.h>

class PWM
{
   public:
      typedef enum
      {
         modePWM,
         modeSerializer
      } Mode;

      PWM(uint8_t* baseAddress);
      ~PWM();

      void enableChannel(uint8_t channel);
      void disableChannel(uint8_t channel);
      void setDutyCycle(uint8_t channel, uint8_t dutyCycle);
      void setPWMMode(uint8_t channel);
      void setMarkSpaceMode(uint8_t channel);

      static uint8_t* const PWM0BaseAddress;

   private:
      uint8_t* baseAddress;
};

#endif
