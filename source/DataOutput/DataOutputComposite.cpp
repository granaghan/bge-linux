#include "DataOutput/DataOutputComposite.h"

DataOutputComposite::DataOutputComposite()
{}

DataOutputComposite::~DataOutputComposite()
{}

void DataOutputComposite::setTemperature(float temperature)
{
   for(std::vector<DataOutput*>::iterator it = dataOutputVector.begin(); it != dataOutputVector.end(); ++it)
   {
      (*it)->setTemperature(temperature);
   }
}

void DataOutputComposite::send()
{
   for(std::vector<DataOutput*>::iterator it = dataOutputVector.begin(); it != dataOutputVector.end(); ++it)
   {
      (*it)->send();
   }
}

void DataOutputComposite::add(DataOutput& dataOutputRef)
{
   dataOutputVector.push_back(&dataOutputRef);
}