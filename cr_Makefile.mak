# Makefile.mak

# IMPORTANT # # # # #
# This makefile assumes you have the arm-none-eabi build tools in the
# PATH environment variable. Also assumes they are the Codebench tools.

# IMPORTANT # # # # #
# The GDB files included by default assumes you have a gdb server already
# running. See the .gdb files in src for more info.

# Main output. Keep it in the output directory. You will have to modify
# the gdb script to program the correct file.
OUTPUT = output/lpc17xxtest

# Make an object file entry for each c file.
OBJ = build/main.o 
OBJ += build/timer3.o
#OBJ += build/pwm.o
#OBJ += build/ssp.o
OBJ += build/uart0.o 
OBJ += build/pin_ops.o
OBJ += build/timer.o
OBJ += build/apbio.o
#OBJ += build/25PE20VP.o
OBJ += build/sd.o
#OBJ += build/audio.o 
OBJ += build/ssp.o
OBJ += build/syscalls.o 
OBJ += build/printf.o
#OBJ += build/math.o 
#OBJ += build/filters.o 
#Drivers from CMSIS
#OBJ += build/i2c.o 
#OBJ += build/lpc17xx_clkpwr.o 
#FS:
OBJ += build/ff.o build/diskio.o build/fattime.o

#OBJ += build/tinysh.o
#OBJ += build/shell_commands.o
#Webstuff
#OBJ += build/emac.o build/httpd.o build/httpd-cgi.o build/httpd-fs.o build/http-strings.o build/psock.o build/enet_timer.o build/uip.o build/uip_arp.o build/upIP_Task.o build/ParTest.o


#shellstuff
#OBJ += build/ntlibc.o build/ntopt.o build/ntshell.o build/text_editor.o build/text_history.o build/vtparse.o build/vtparse_table.o

#audio
OBJ += build/audio.o build/i2c.o build/lpc17xx_clkpwr.o

# Startup code
OBJ += build/system_LPC17xx.o 
#Code Red Startup code
OBJ += build/cr_startup_LPC17xx.o
#OBJ += build/startup_LPC17xx.o
# FreeRTOS files. Uncomment if needed.
OBJ += build/tasks.o build/list.o build/croutine.o build/port.o build/queue.o build/timers.o build/heap_2.o build/handlers.o build/hooks.o

#APB FSM

OBJ +=  build/apbconfig.o build/apbfsmio.o build/apbfsm.o
# Linker script
#LSCRIPT = src/LPC17xx.ld

#Code Red Linker Script
LSCRIPT = src/rtosdemo_rdb1768_Debug.ld

CFLAGS = -g -c -Wall -Wextra -fno-common -mcpu=cortex-m3 -mthumb 
#CFLAGS = -c -g -fno-common -mcpu=cortex-m3 -mthumb -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D inline=
CFLAGS += -IFreeRTOS -Isrc -IFilesystem -IUART -Ishell -Iapb_core -IAudio
LFLAGS = -mcpu=cortex-m3 -mthumb -nostartfiles -Wl,-Map=$(OUTPUT).map -T$(LSCRIPT) $(LIBS)
AFLAGS = -mcpu=cortex-m3 

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
RM = rm -f
GDB = arm-none-eabi-gdb

.PHONY: all debug program clean distclean stats

all: $(OUTPUT).hex $(OUTPUT).bin $(LSCRIPT)
	cp output/lpc17xxtest.bin /media/Drag2FLASH/
#	scp output/lpc17xxtest.bin swen3027@5.209.107.16:/home/swen3027/
debug: $(OUTPUT).elf
	$(GDB) -x src/LPC17xx.gdb

program: $(OUTPUT).hex
	$(GDB) --batch -x src/LPC17xx_prog.gdb

$(OUTPUT).bin: $(OUTPUT).elf
	$(OBJCOPY) -O binary -j .text -j .data $(OUTPUT).elf $(OUTPUT).bin

$(OUTPUT).hex: $(OUTPUT).elf
	$(OBJCOPY) -R .stack -O ihex $(OUTPUT).elf $(OUTPUT).hex

$(OUTPUT).elf: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $(OUTPUT).elf

stats: $(OUTPUT).elf
	$(SIZE) $(OUTPUT).elf

clean:
	$(RM) -f build/*

distclean:
	$(RM) $(OUTPUT).hex
	$(RM) $(OUTPUT).elf
	$(RM) $(OUTPUT).map
	$(RM) $(OUTPUT).bin
	
# Put your build rules here.
#APB FSM

build/apbconfig.o: apb_core/apbconfig.c
	$(CC) $(CFLAGS) -o $@ $<	
build/apbfsm.o: apb_core/apbfsm.c
	$(CC) $(CFLAGS) -o $@ $<
build/apbfsmio.o: apb_core/apbfsmio.c
	$(CC) $(CFLAGS) -o $@ $<
build/apbio.o: apb_core/apbio.c
	$(CC) $(CFLAGS) -o $@ $<
		

build/main.o: src/main.c
	$(CC) $(CFLAGS) -o $@ $<
build/shell_commands.o: shell/shell_commands.c
	$(CC) $(CFLAGS) -o $@ $<
build/tinysh.o: shell/tinysh.c
	$(CC) $(CFLAGS) -o $@ $<
build/timer3.o: src/timer3.c
	$(CC) $(CFLAGS) -o $@ $<
#audio

build/audio.o: Audio/audio.c
	$(CC) $(CFLAGS) -o $@ $<
build/i2c.o: Audio/i2c.c
	$(CC) $(CFLAGS) -o $@ $<

build/lpc17xx_clkpwr.o: Audio/lpc17xx_clkpwr.c
	$(CC) $(CFLAGS) -o $@ $<
#build/pwm.o: src/pwm.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/ssp.o: src/ssp.c
#	$(CC) $(CFLAGS) -o $@ $<
build/pin_ops.o: src/pin_ops.c
	$(CC) $(CFLAGS) -o $@ $<
build/timer.o: src/timer.c
	$(CC) $(CFLAGS) -o $@ $<
#build/apbio.o: src/apbio.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/25PE20VP.o: src/25PE20VP.c
#	$(CC) $(CFLAGS) -o $@ $<
build/uart0.o: UART/uart0.c
	$(CC) $(CFLAGS) -o $@ $<
#FS stuff
build/ssp.o: Filesystem/ssp.c
	$(CC) $(CFLAGS) -o $@ $<	
build/sd.o: Filesystem/sd.c
	$(CC) $(CFLAGS) -o $@ $<	
build/ff.o: Filesystem/ff.c
	$(CC) $(CFLAGS) -o $@ $<
build/diskio.o: Filesystem/diskio.c
	$(CC) $(CFLAGS) -o $@ $<
build/fattime.o: Filesystem/fattime.c
	$(CC) $(CFLAGS) -o $@ $<
	
#webstuffs

#build/emac.o: webserver/emac.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/httpd.o: webserver/httpd.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/httpd-cgi.o: webserver/httpd-cgi.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/httpd-fs.o: webserver/httpd-fs.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/ParTest.o: webserver/ParTest.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/http-strings.o: webserver/http-strings.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/psock.o: webserver/psock.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/enet_timer.o: webserver/enet_timer.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/uip.o:webserver/uip.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/uip_arp.o: webserver/uip_arp.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/upIP_Task.o: webserver/uIP_Task.c
#	$(CC) $(CFLAGS) -o $@ $<







#build/audio.o: src/audio.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/math.o: src/math.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/filters.o: src/filters.c
#	$(CC) $(CFLAGS) -o $@ $<
	
#build/lpc17xx_i2s.o: src/lpc17xx_i2s.c
#	$(CC) $(CFLAGS) -o $@ $<

#build/i2c.o: src/i2c.c
#	$(CC) $(CFLAGS) -o $@ $<
#build/lpc17xx_clkpwr.o: src/lpc17xx_clkpwr.c
#	$(CC) $(CFLAGS) -o $@ $<
	
# Startup file build rules.
#build/startup_LPC17xx.o: src/startup_LPC17xx.s
#	$(AS) $(AFLAGS) -o $@ $<

#Code Red startup code
build/cr_startup_LPC17xx.o: src/cr_startup_lpc17.c
	$(CC) $(CFLAGS) -o $@ $<

build/syscalls.o: src/syscalls.c
	$(CC) $(CFLAGS) -o $@ $<

build/system_LPC17xx.o: src/system_LPC17xx.c
	$(CC) $(CFLAGS) -o $@ $<
build/core_cm3.o: src/core_cm3.c
	$(CC) $(CFLAGS) -o $@ $<	
build/handlers.o: src/handlers.c
	$(CC) $(CFLAGS) -o $@ $<
build/hooks.o: src/hooks.c
	$(CC) $(CFLAGS) -o $@ $<
build/printf.o: UART/printf.c
	$(CC) $(CFLAGS) -o $@ $<
	
# FreeRTOS build rules.
build/tasks.o: FreeRTOS/tasks.c
	$(CC) $(CFLAGS) -o $@ $<
build/list.o: FreeRTOS/list.c
	$(CC) $(CFLAGS) -o $@ $<
build/croutine.o: FreeRTOS/croutine.c
	$(CC) $(CFLAGS) -o $@ $<
build/port.o: FreeRTOS/port.c
	$(CC) $(CFLAGS) -o $@ $<
build/queue.o: FreeRTOS/queue.c
	$(CC) $(CFLAGS) -o $@ $<
build/timers.o: FreeRTOS/timers.c
	$(CC) $(CFLAGS) -o $@ $<
build/heap_2.o: FreeRTOS/heap_2.c
	$(CC) $(CFLAGS) -o $@ $<	
