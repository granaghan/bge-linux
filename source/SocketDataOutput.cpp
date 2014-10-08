#include "SocketDataOutput.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


SocketDataOutput::SocketDataOutput():
   socket(io_service),
   newData(false),
   temperature(0.0f)
{
   socket.open(boost::asio::ip::udp::v4());
   remote_endpoint = boost::asio::ip::udp::endpoint
   (
      boost::asio::ip::address(boost::asio::ip::address_v4::loopback()), //boost::asio::ip::address(boost::asio::ip::address::from_string("0.0.0.0")),
      1234
   );
   //socket.bind(remote_endpoint);
}

SocketDataOutput::~SocketDataOutput()
{

}

void SocketDataOutput::setTemperature(float temperature)
{
   newData = true;
   this->temperature = temperature;
}

void SocketDataOutput::send()
{
   if(newData)
   {
      char str[255] = "";
      sprintf(str, "{\"date\":%d, \"temperature\":%lf}\n", time(NULL), temperature);
      socket.send_to(boost::asio::buffer(str, strlen(str)), remote_endpoint);
      newData = false;
   }
}