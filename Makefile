PRJ = /home/alex/source/simple_sched_sem
CCPATH = /home/alex/tool/gcc-arm-none-eabi-10.3-2021.10/bin

########################################################################
CC=$(CCPATH)/arm-none-eabi-gcc
LD=$(CCPATH)/arm-none-eabi-gcc
CP=$(CCPATH)/arm-none-eabi-objcopy

########################################################################
LDSCRIPT = -T$(PRJ)/STM32F746NGHx_FLASH.ld

########################################################################
INC += -I$(PRJ)/Drivers/STM32F7xx_HAL_Driver/Inc
INC += -I$(PRJ)/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
INC += -I$(PRJ)/Drivers/CMSIS/Device/ST/STM32F7xx/Include
INC += -I$(PRJ)/Drivers/CMSIS/Include

SRC += $(PRJ)/Src/main.c
SRC += $(PRJ)/Src/sem.c
SRC += $(PRJ)/Src/system_stm32f7xx.c

ASM += $(PRJ)/startup_stm32f746xx.s
ASM += $(PRJ)/Src/sched.s

#######################################################################
OBJ=$(SRC:%.c=%.o)
OBJ+=$(ASM:%.s=%.o)

#######################################################################
CFLAGS += -mcpu=cortex-m7
CFLAGS += -mlittle-endian
CFLAGS += -mthumb
CFLAGS += -g -O0
CFLAGS += $(INC)

LDFLAGS += -mcpu=cortex-m7
LDFLAGS += -mlittle-endian
LDFLAGS += -mthumb
LDFLAGS += $(LDSCRIPT)
LDFLAGS += -Wl,--gc-section

CDEFS = -DSTM32F746xx

######################################################################

all: start
	@echo "\n"
	@echo "END"

start: main.elf
	@echo "\n"
	@echo "OUT"
	@echo "********************************************************"
	$(CP) -Oihex main.elf main.hex
	@echo "********************************************************"

main.elf: $(OBJ)
	@echo "\n"
	@echo "LINK"
	@echo "********************************************************"
	$(LD) $(LDFLAGS) $(OBJ) -o main.elf
	@echo "********************************************************"

%.o: %.c
	@echo "\n"
	@echo "COMPILE C"
	@echo "********************************************************"
	$(CC) $(CFLAGS) $(CDEFS) -c $< -o $@
	@echo "********************************************************"

%.o: %.s
	@echo "\n"
	@echo "COMPILE S"
	@echo "********************************************************"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "********************************************************"

clean:
	rm -f $(OBJ) main.elf main.hex
