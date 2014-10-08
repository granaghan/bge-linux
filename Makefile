# Force deault target
default: temp | dirs
codegen: dirs

# Define toolchain variables
TOOLCHAIN=
CC=$(TOOLCHAIN)gcc
CXX=$(TOOLCHAIN)g++
AS=$(TOOLCHAIN)as
LD=$(TOOLCHAIN)ld
OBJCOPY=$(TOOLCHAIN)objcopy
ARCH=$(TOOLCHAIN)
PYTHON=python

CODEGEN_DIR:=$(CURDIR)/codegen
MK_DIRS:= $(CODEGEN_DIR)

define addRegmap
ifeq ($($(basename $(notdir $1))_READFUNC),)
$(basename $(notdir $1))_READFUNC:=read32
endif
ifeq ($($(basename $(notdir $1))_WRITEFUNC),)
$(basename $(notdir $1))_WRITEFUNC:=write32
endif
ARM_SRC+=$(CODEGEN_DIR)/$(basename $(notdir $1)).c
codegen:$(CODEGEN_DIR)/$(basename $(notdir $1)).c
codegen:$(CODEGEN_DIR)/$(basename $(notdir $1)).h
$(CODEGEN_DIR)/$(basename $(notdir $1)).c:$1
	@echo $(notdir $$@) \<-- $(notdir $$<)
	@$(PYTHON) source/regmaps/RegMapParse.py $$< | python source/regmaps/GenerateRegisterMap.py --include "io.h" --read_func $$($(basename $(notdir $1))_READFUNC) --write_func $$($(basename $(notdir $1))_WRITEFUNC) > $$@
$(CODEGEN_DIR)/$(basename $(notdir $1)).h:$1
	@echo $(notdir $$@) \<-- $(notdir $$<)
	@$(PYTHON) source/regmaps/RegMapParse.py $$< | python source/regmaps/GenerateRegisterMap.py --header > $$@
endef

#
# CFLAGS common to both the THUMB and ARM mode builds
#
CFLAGS=-Wall -Wextra -Isource/FreeRTOS/Source/include \
		-Isource -I. $(DEBUG) -lboost_thread-mt -lboost_system -march=armv6 -mfpu=vfp -mfloat-abi=hard \
		-g -fno-dwarf2-cfi-asm -O0

      #-Wcast-align $(OPTIM)

ifeq ($(USE_THUMB_MODE),YES)
	CFLAGS += -mthumb-interwork -D THUMB_INTERWORK
	THUMB_FLAGS=-mthumb
endif

LINKER_FLAGS=-Xlinker -otemp -Xlinker -M -Xlinker -Map=temp.map

#
# Source files that can be built to THUMB mode.
#
THUMB_SRC = \
   source/main.c \

#
# Source files that must be built to ARM mode.
#
ARM_CPP_SRC :=\
   source/Drivers/UART.cpp \
   source/Drivers/GPIO.cpp \
   source/Drivers/SPI.cpp \
   source/Drivers/PWM.cpp \
   source/Drivers/Clock.cpp \
   source/Drivers/I2C.cpp \
   source/Drivers/SystemTimer.cpp \
   source/Peripherals/SparkfunLCD.cpp \
   source/Peripherals/MAX31855.cpp \
   source/tasks.cpp \
   source/DataOutputComposite.cpp \
   source/NamedPipeDataOutput.cpp \
   source/SocketDataOutput.cpp \

ARM_SRC := \
   source/Drivers/interrupts.c \
   source/io.c \

# Define regmap source and call generation rule
REGMAPS := \
   source/regmaps/SPI.rm \
   source/regmaps/UART.rm \
   source/regmaps/GPIO.rm \
   source/regmaps/PWM.rm \
   source/regmaps/CLOCK.rm \
   source/regmaps/I2C.rm \
   source/regmaps/SYSTIMER.rm \

CLOCK_WRITEFUNC=write32Clk

$(foreach regmap,$(REGMAPS),$(eval $(call addRegmap,$(regmap))))

#
# Define all object files.
#
ARM_OBJ := $(ARM_SRC:.c=.o)
ARM_CPP_OBJ := $(ARM_CPP_SRC:.cpp=.o)
THUMB_OBJ := $(THUMB_SRC:.c=.o)

temp : $(ARM_OBJ) $(ARM_CPP_OBJ) $(THUMB_OBJ) codegen | dirs
	@echo $(notdir $@)
	@$(CXX) $(CFLAGS) $(ARM_OBJ) $(ARM_CPP_OBJ) $(THUMB_OBJ) $(LINKER_FLAGS)
	@echo

   #$(LD) $(ARM_OBJ) $(THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)  raspberrypi.ld

$(THUMB_OBJ) : %.o : %.c
	@echo $(notdir $@) \<-- $(notdir $<)
	@$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@
	@echo

$(ARM_OBJ) : %.o : %.c
	@echo $(notdir $@) \<-- $(notdir $<)
	@$(CC) -c $(CFLAGS) $< -o $@
	@echo

$(ARM_CPP_OBJ) : %.o : %.cpp
	@echo $(notdir $@) \<-- $(notdir $<)
	@$(CXX) -c $(CFLAGS) $< -o $@
	@echo

dirs:
	@mkdir -p $(MK_DIRS)

clean :
	@rm -f $(ARM_CPP_OBJ)
	@rm -f $(ARM_OBJ)
	@rm -f $(THUMB_OBJ)
	@rm -rf $(CODEGEN_DIR)
