extern "C"
{
#include "FreeRTOS.h"
#include <task.h>
}

// Required functionality for C++ features. I'm not 100% sure that these
// are correct though.
// See the following:
// http://wiki.osdev.org/C%2B%2B
// http://mentorembedded.github.io/cxx-abi/abi.html

// I don't believe that this should ever get used in an embedded ARM application
// as global/static objects should never be destructed.
void* __dso_handle;

typedef void (*constructorPtr)(void);

extern constructorPtr _init_array_start[0], _init_array_end[0];
void _init(void)
{
   for(constructorPtr* constructor = _init_array_start; constructor != _init_array_end; constructor++)
   {
      (*constructor)();
   }
}


namespace __cxxabiv1
{
	/* guard variables */

	/* The ABI requires a 64-bit type.  */
	__extension__ typedef int __guard __attribute__((mode(__DI__)));

	extern "C" int __cxa_guard_acquire (__guard *);
	extern "C" void __cxa_guard_release (__guard *);
	extern "C" void __cxa_guard_abort (__guard *);

   // Enter a critical section so we can't be interrupted.
	extern "C" int __cxa_guard_acquire (__guard *g)
	{
      taskENTER_CRITICAL();
		return !*(char *)(g);
	}

   // Object is always flagged as created. Exit critical section
   // after marking object as created.
	extern "C" void __cxa_guard_release (__guard *g)
	{
		*(char *)g = 1;
      taskEXIT_CRITICAL();
	}

   // Since we never say we failed, there's nothing to do here.
	extern "C" void __cxa_guard_abort (__guard *)
	{

	}
}