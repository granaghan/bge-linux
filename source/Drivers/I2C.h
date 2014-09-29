#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include <codegen/I2C.h>
#include <FreeRTOSConfig.h>
#include <stdint.h>

class I2C
{
   public:
      typedef enum 
      {
         transferModeWrite,
         transferModeRead
      } TransferMode;

      I2C(uint8_t* baseAddress);

      ~I2C();

      void enable();
      void disable();
      void startTransfer();
      void clearFIFO();
      void setTransferMode(TransferMode transferMode);

      bool checkForError();
      bool ackErrorDetected();
      bool clockStretchTimeoutDetected();

      bool txFIFOEmpty();
      bool dataAvailable();

      bool transferDone();
      bool transferActive();

      void setDataLength(uint16_t dataLength);

      // I2C addresses are 7 bits
      void setAddress(uint8_t address);

      void setClockDivider(uint16_t clockDivider);

      void setClockStretchTimeout(uint16_t timeoutLengthInClockCycles);

      void write(uint8_t data);
      uint8_t read();
      
      static uint8_t* const I2C0BaseAddress;
      static uint8_t* const I2C1BaseAddress;
      static uint8_t* const I2C2BaseAddress;

   private:
      uint8_t* baseAddress;
};

#endif
