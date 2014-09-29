#include <stdint.h>

const uint32_t clockPassword = 0x5A << 24;

inline void write32(volatile uint32_t* ptr, uint32_t value)
{
   *ptr = value;
}

inline void write32Clk(volatile uint32_t* ptr, uint32_t value)
{
   *ptr = clockPassword | value;
}

inline uint32_t read32(volatile uint32_t* ptr)
{
   return *ptr;
}
