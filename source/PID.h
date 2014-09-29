#include <stdint.h>

class PID
{
   public:
      PID(float kp, float ki, float kd):
         kp(kp),
         ki(ki),
         kd(kd)
      {
         lastValue = 100.0f;
         setPoint = 82.0f;
         previousError = 0.0f;
         average = 0.0f;
      }
      ~PID(){}

      float processSample(float sample, float dt)
      {
         float currentError = setPoint - sample;
         float derivative = (currentError - previousError) / dt;
         average += currentError*dt;

         if(lastValue > 99.0f)
         {
            average -= currentError*dt;
         }

         previousError = currentError;

         lastValue = kp*currentError + ki*average + kd*derivative;

         if (lastValue > 100.0f)
         {
            lastValue = 100.0f;
         }
         else if(lastValue < 0.0f)
         {
            lastValue = 0.0f;
         }

         return lastValue;
      }

   private:
      float kp;
      float ki;
      float kd;
      float setPoint;
      float average;
      float previousError;
      float lastValue;
};