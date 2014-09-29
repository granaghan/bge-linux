#ifndef PERIPHERALS_SPARKFUNLCD_H_
#define PERIPHERALS_SPARKFUNLCD_H_
#include <Drivers/UART.h>
#include <stdint.h>

class SparkfunLCD
{
   public:
      SparkfunLCD(UART& uart);
      ~SparkfunLCD();

      // Sets a brightness level 0-100.
      void setBacklightBrightness(uint8_t level);

      void clear();

      void cursorLeft();
      void cursorRight();
      void setCursorPosition(uint8_t position);

      void scrollLeft();
      void scrollRight();

      void enableCursorUnderline(bool enable);
      void enableCursorBlink(bool enable);

      void sendCharacter(char c);
      // str must be a null-terminated C string.
      void sendString(const char* str);

   private:
      UART& uart;

      static const uint8_t commandChar              = 0xFE;
      static const uint8_t altCommandChar           = 0x7C;
      static const uint8_t clearCommand             = 0x01;
      static const uint8_t moveCursorLeftCommand    = 0x10;
      static const uint8_t moveCursorRightCommand   = 0x14;
      static const uint8_t setCursorCommand         = 0x80;
      static const uint8_t scrollCursorLeftCommand  = 0x18;
      static const uint8_t scrollCursorRightCommand = 0x1C;
      static const uint8_t underlineOnCommand       = 0x0E;
      static const uint8_t underlineOffCommand      = 0x0C;
      static const uint8_t boxOnCommand             = 0x0D;
      static const uint8_t boxOffCommand            = 0x0C;
};

#endif