<%
   def getMask(width):
      return 2**width-1
%>\
%for include in options.includes:
#include "${include}"
%endfor
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

%for reg in regs:
void write${reg['name']}(uint8_t* offset, uint32_t value)${';' if options.header else ' '}
%if not options.header:
{
   ${options.write_func}(((volatile uint32_t*)( ((uint8_t*)offset) + ${hex(reg['offset'])} ) ), value);
}
%endif

uint32_t read${reg['name']}(uint8_t* offset)${';' if options.header else ' '}
%if not options.header:
{
   return ${options.read_func}(((volatile uint32_t*)( ((uint8_t*)offset) + ${hex(reg['offset'])} ) ));
}
%endif

%for field in reg['fields']:
%if field['name'] != 'NULL':
void write${reg['name']}_${field['name']}(uint8_t* offset, uint32_t value)${';' if options.header else ' '}
%if not options.header:
{
   uint32_t regValue = ${options.read_func}(((volatile uint32_t*) ( ((uint8_t*)offset) + ${hex(reg['offset'])} ) ));
   regValue &= ~(${hex(getMask(field['width']))} << ${field['offset']});
   regValue |= (value & ${hex(getMask(field['width']))}) << ${field['offset']};
   ${options.write_func}(((volatile uint32_t*) ( ((uint8_t*)offset) + ${hex(reg['offset'])} ) ), regValue);
}
%endif

uint32_t read${reg['name']}_${field['name']}(uint8_t* offset)${';' if options.header else ' '}
%if not options.header:
{
   uint32_t regValue = ${options.read_func}(((volatile uint32_t*)( ((uint8_t*)offset) + ${hex(reg['offset'])} ) ));
   regValue = (regValue >> ${field['offset']}) & ${hex(getMask(field['width']))};
   return regValue;
}
%endif
%endif

%endfor
%endfor
#ifdef __cplusplus
}
#endif