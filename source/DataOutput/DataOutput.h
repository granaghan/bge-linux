#ifndef DATAOUTPUT_H_
#define DATAOUTPUT_H_

class DataOutput
{
   public:
      DataOutput(){}
      ~DataOutput(){}

      virtual void setTemperature(float temperature)=0;
      virtual void send()=0;
};

#endif
