extern "C"
{
#include <codegen/SPI.h>
#include "tasks.h"
}
//#include <boost/lockfree/queue.hpp>
#include <queue>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "PID.h"
#include "Drivers/Clock.h"
#include "Drivers/GPIO.h"
#include "Drivers/I2C.h"
#include "Drivers/PWM.h"
#include "Drivers/SPI.h"
#include "Drivers/SystemTimer.h"
#include "Drivers/UART.h"
#include "Peripherals/SparkfunLCD.h"
#include "Peripherals/MAX31855.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

const signed char* task1Str = reinterpret_cast<const signed char*>("task1");
const signed char* task2Str = reinterpret_cast<const signed char*>("task2");

typedef struct
{
   SystemTimer& timerRef;
   std::queue<float>& temperatureQueueHandle;
   std::queue<uint32_t>& fanLevelQueueHandle;
   SparkfunLCD& lcdRef;
   MAX31855& max31855Ref;
} TaskStruct;

extern "C" void initializePlatform()
{
   int mem_fd = 0;
   void* map;
   if((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
   {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
   map = mmap(reinterpret_cast<void*>(0x20200000), 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0x20200000);
   printf("0: 0x%p\n", map);
   close(mem_fd);

   GPIO& gpio = GPIO::getSingleton();
   gpio.setPinFunction(readyLEDPin, GPIO::pinFunctionOutput);
   gpio.setPinFunction(uartTxPin, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(uartRxPin, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(7, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(8, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(9, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(10, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(11, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(0, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(1, GPIO::pinFunctionAlternate0);
   gpio.setPinFunction(18, GPIO::pinFunctionAlternate5);
   gpio.setPinPullDirection(uartTxPin, GPIO::pullDirectionDown);
   gpio.setPinPullDirection(uartRxPin, GPIO::pullDirectionDown);
   gpio.setPinPullDirection(0, GPIO::pullDirectionUp);
   gpio.setPinPullDirection(1, GPIO::pullDirectionUp);
   //gpio.setPinPullDirection(7, GPIO::pullDirectionUp);
   //gpio.setPinPullDirection(8, GPIO::pullDirectionUp);
   //gpio.setPinPullDirection(9, GPIO::pullDirectionUp);
   //gpio.setPinPullDirection(10, GPIO::pullDirectionUp);
   //gpio.setPinPullDirection(11, GPIO::pullDirectionUp);
   //gpio.setPinLevel(readyLEDPin, GPIO::pinLevelLow);
}

uint8_t* i2cMappedBase = reinterpret_cast<uint8_t*>(NULL);
uint8_t* pwmClockMappedBase = reinterpret_cast<uint8_t*>(NULL);
uint8_t* pwmMappedBase = reinterpret_cast<uint8_t*>(NULL);

extern "C" void spawnTasks()
{
   int mem_fd = 0;
   void* map;
   if((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
   {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   std::queue<float> temperatureQueue;
   std::queue<uint32_t> fanLevelQueue;

   // The following mmaps are temporary and evil. Don't do this.
   map = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(SystemTimer::SystemTimerBaseAddress));
   printf("1: 0x%p\n", map);
   SystemTimer timer(static_cast<uint8_t*>(map));

   map = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(UART::UART0BaseAddress));
   printf("2: 0x%p\n", map);
   UART uart(static_cast<uint8_t*>(map));

   map = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(SPI::SPI0BaseAddress));
   printf("3: 0x%p\n", map);
   SPI spi(static_cast<uint8_t*>(map));

   i2cMappedBase = reinterpret_cast<uint8_t*>(mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(I2C::I2C0BaseAddress)));
   printf("4: 0x%p\n%d\n", i2cMappedBase, errno);
   pwmClockMappedBase = reinterpret_cast<uint8_t*>(mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(Clock::PWMClockBaseAddress) & ~0xFFFF));
   printf("5: 0x%p\n%d\n", pwmClockMappedBase, errno);
   pwmMappedBase = reinterpret_cast<uint8_t*>(mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, reinterpret_cast<__off_t>(PWM::PWM0BaseAddress)));
   printf("6: 0x%p\n%d\n", pwmMappedBase, errno);
   close(mem_fd);

   spi.setChipSelectPolarity(SPI::polarityLow);
   spi.setClockRate(976000);

   SparkfunLCD lcd(uart);
   MAX31855 temperatureReader(spi, 0);
   TaskStruct taskStruct = {timer, temperatureQueue, fanLevelQueue, lcd, temperatureReader};

   printf("starting threads \n");
   boost::thread task1Thread(task1, &taskStruct);
   boost::thread task3Thread(task3, &taskStruct);
   printf("started threads\n");
   while(-1);
}

extern "C" void statusOn()
{
   GPIO& gpio = GPIO::getSingleton();
   gpio.setPinLevel(readyLEDPin, GPIO::pinLevelLow);
}

extern "C" void statusOff()
{
   GPIO& gpio = GPIO::getSingleton();
   gpio.setPinLevel(readyLEDPin, GPIO::pinLevelHigh);
}

extern "C" void task1(void *pParam)
{
   printf("task1 started\n");
   boost::posix_time::milliseconds sleepTime(500);
   GPIO& gpio = GPIO::getSingleton();
   PID pid(5.0f,5.0f,5.0f);
   float fanLevel = 0.0f;
   uint32_t integerFanLevel = 0;
   MAX31855& temperatureReader = reinterpret_cast<TaskStruct*>(pParam)->max31855Ref;
   std::queue<float>& temperatureQueue = reinterpret_cast<TaskStruct*>(pParam)->temperatureQueueHandle;
   std::queue<uint32_t>& fanLevelQueue = reinterpret_cast<TaskStruct*>(pParam)->fanLevelQueueHandle;

   uint32_t offsetBase = reinterpret_cast<uint32_t>(pwmClockMappedBase) + (reinterpret_cast<uint32_t>(Clock::PWMClockBaseAddress) & 0xFFFF);
   Clock pwmClock(reinterpret_cast<uint8_t*>(offsetBase));
   pwmClock.kill();
   pwmClock.setDivisor(100, 0);
   pwmClock.setMASHControl(Clock::MASH1Stage);
   pwmClock.setClockSource(Clock::clockSourcePLLD);
   pwmClock.enable();


   PWM pwm(pwmMappedBase);
   int i = 0;
   float temperature = 0.0f;
   pwm.setDutyCycle(0, 75);
   pwm.setPWMMode(0);

   pwm.enableChannel(0);
   while(1)
   {
      //printf("task1 main loop\n");
      temperature = temperatureReader.readTemperature();
      temperature = temperature * 9.0f/5.0f + 32.0f;
      fanLevel = pid.processSample(temperature, 1.0f);
      integerFanLevel = fanLevel;
      temperatureQueue.push(temperature);
      fanLevelQueue.push(fanLevel);
      printf("temp: %f\n", temperature);

      i++;
      if(i&1)
      {
         gpio.setPinLevel(readyLEDPin, GPIO::pinLevelLow);
      }
      else
      {
         gpio.setPinLevel(readyLEDPin, GPIO::pinLevelHigh);
      }

      if(temperature > 26)
      {
         pwm.setDutyCycle(0, 10);
      }
      else
      {
         pwm.setDutyCycle(0, 75);
      }

      boost::this_thread::sleep(sleepTime);
   }
}

extern "C" void taskBlink(void *pParam)
{
   boost::posix_time::milliseconds sleepTime(500);
   GPIO& gpio = GPIO::getSingleton();
   int i = 0;

   while(1)
   {
      i++;
      if(i&1)
      {
         gpio.setPinLevel(readyLEDPin, GPIO::pinLevelLow);
      }
      else
      {
         gpio.setPinLevel(readyLEDPin, GPIO::pinLevelHigh);
      }

      boost::this_thread::sleep(sleepTime);
   }
}

extern "C" void task3(void *pParam)
{
   printf("task3 started\n");
   statusOn();
   boost::posix_time::milliseconds sleepTime(500);
   GPIO& gpio = GPIO::getSingleton();
   gpio.setPinLevel(readyLEDPin, GPIO::pinLevelHigh);

   SystemTimer& timer = reinterpret_cast<TaskStruct*>(pParam)->timerRef;
   SparkfunLCD& lcd = reinterpret_cast<TaskStruct*>(pParam)->lcdRef;
   MAX31855& temperatureReader = reinterpret_cast<TaskStruct*>(pParam)->max31855Ref;
   std::queue<float>& temperatureQueue = reinterpret_cast<TaskStruct*>(pParam)->temperatureQueueHandle;
   std::queue<uint32_t>& fanLevelQueue = reinterpret_cast<TaskStruct*>(pParam)->fanLevelQueueHandle;

   I2C i2c(i2cMappedBase);


   i2c.setAddress(1);
   i2c.setTransferMode(I2C::transferModeRead);
   i2c.setClockDivider(2500); // 100kHz
   i2c.setDataLength(1);
   //i2c.setClockStretchTimeout(255);
   i2c.clearFIFO();
   i2c.enable();

   uint32_t temperature = 0;
   uint32_t fanLevel = 0;
   uint64_t timeElapsed;
   char strBuffer[48];

   lcd.setBacklightBrightness(20);
   lcd.enableCursorBlink(true);

   while(1)
   {
      //printf("task3 main loop\n");
      snprintf(strBuffer, 48, "test");
      lcd.sendString(strBuffer);
      if(!temperatureQueue.empty())
      {
         temperature = temperatureQueue.back();
         temperatureQueue.pop();
         lcd.setCursorPosition(1);
         snprintf(strBuffer, 48, "%d", temperature);
         //lcd.clear();
         lcd.sendString(strBuffer);
      }

      if(!fanLevelQueue.empty())
      {
         fanLevel = fanLevelQueue.back();
         fanLevelQueue.pop();
         lcd.setCursorPosition(8);
         snprintf(strBuffer, 48, " %d%%", fanLevel);
         //lcd.clear();
         lcd.sendString(strBuffer);
      }

      lcd.setCursorPosition(15);
      timeElapsed =  timer.getTimeInSeconds();
      snprintf(strBuffer, 48, " %lld", timeElapsed / 60);
      lcd.sendString(strBuffer);

      snprintf(strBuffer, 48, ":%02lld", timeElapsed % 60);
      lcd.sendString(strBuffer);

      i2c.startTransfer();
      while(i2c.dataAvailable() && !i2c.checkForError())
      {
         snprintf(strBuffer, 48, " %d", i2c.read());
         //lcd.sendString(strBuffer);
      }
      if(i2c.ackErrorDetected())
      {
         lcd.sendString(" ack error");
      }
   }
}
