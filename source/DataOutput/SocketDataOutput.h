#ifndef SOCKETDATAOUTPUT_H_
#define SOCKETDATAOUTPUT_H_

#include "DataOutput/DataOutput.h"
#include <boost/asio.hpp>

class SocketDataOutput: public virtual DataOutput
{
   public:
      SocketDataOutput();
      ~SocketDataOutput();

      virtual void setTemperature(float temperature);
      virtual void send();
      void add(DataOutput& dataOutputRef);

   private:
      boost::asio::io_service io_service;
      boost::asio::ip::udp::socket socket;
      boost::asio::ip::udp::endpoint remote_endpoint;
      bool newData;
      float temperature;

};

#endif
