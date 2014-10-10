#ifndef NAMEDPIPEDATAOUTPUT_H_
#define NAMEDPIPEDATAOUTPUT_H_

#include "DataOutput/DataOutput.h"

class NamedPipeDataOutput: public virtual DataOutput
{
   public:
      NamedPipeDataOutput(const char* name);
      ~NamedPipeDataOutput();

      virtual void setTemperature(float temperature);
      virtual void send();

   private:
      int fd;
      float temperature;
};

#endif
