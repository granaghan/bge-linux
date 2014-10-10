#include "DataOutput/NamedPipeDataOutput.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


NamedPipeDataOutput::NamedPipeDataOutput(const char* name)
{
   mkfifo(name, 0666);
   fd = open(name, O_WRONLY);
}

NamedPipeDataOutput::~NamedPipeDataOutput()
{
   close(fd);
}

void NamedPipeDataOutput::setTemperature(float temperature)
{
   this->temperature = temperature;
}

void NamedPipeDataOutput::send()
{
   char str[255] = "";
   sprintf(str, "{\"date\":%ud, \"temperature\":%lf}\n", static_cast<unsigned int>(time(NULL)), temperature);
   write(fd, str, strlen(str));
}