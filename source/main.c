#include "Drivers/interrupts.h"
#include "tasks.h"

void main(void)
{

   initializePlatform();
   spawnTasks();

   while(-1);
}
