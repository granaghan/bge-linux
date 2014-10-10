#ifndef DATAOUTPUTCOMPOSITE_H_
#define DATAOUTPUTCOMPOSITE_H_

#include "DataOutput/DataOutput.h"
#include <vector>

class DataOutputComposite: public virtual DataOutput
{
   public:
      DataOutputComposite();
      ~DataOutputComposite();

      virtual void setTemperature(float temperature);
      virtual void send();
      void add(DataOutput& dataOutputRef);

   private:
      std::vector<DataOutput*> dataOutputVector;
};

#endif
