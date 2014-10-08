#ifndef DATAPIPE_H_
#define DATAPIPE_H_

class DataPipe
{
   public:
      DataPipe(const char* name);
      ~DataPipe();

      void sendTemperature(float temperature);

   private:
      int fd;
};

#endif
