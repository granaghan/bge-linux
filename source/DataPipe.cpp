#include "DataPipe.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


DataPipe::DataPipe(const char* name)
{
   mkfifo(name, 0666);
   fd = open(name, O_WRONLY);
}

DataPipe::~DataPipe()
{
   close(fd);
}

void DataPipe::sendTemperature(float temperature)
{
   char str[255] = "";
   sprintf(str, "{\"date\":%d, \"temperature\":%lf}\n", time(NULL), temperature);
   write(fd, str, strlen(str));
}