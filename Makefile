# Simple makefile for building the 
# Arduino amforth vor various targets

# Examples of usage for Arduino leonardo:
#
# 1) Assemble the whole flash and eemprom files
#     make leonardo.hex
#
# 2) Backup the current flash & eeprom values 
#     make leonardo.bak
#
# 3) Erase the whole MCU Flash
#    make leonardo.era
#
# 4) Upload the new firmware using the hex file generated
#    make leonardo
#
# 5) Set the appropiate MCU fuses
#    make leonardo.fuse
#
# 6) Clear files (except backup)
#    make leonardo.clr


SHELL=/bin/bash

##############################
# TARGET DEPENDANT VARIABLES #
##############################

# 1) MCU should be identical to the device
#    Look at the /core/devices/ folder
# 2) PART is the device model passed to avrdude.
# 3) LFUSE, HFUSE, EFUSE are the device-specific fuses
#    there is a useful fuse calc tool at:
#    http://www.engbedded.com/fusecalc/
# --------------------------------------
# Example fuse settings for 'leonardo'
# Low Fuse LFUSE=0xFF
#  - No Div8 prescaler, 
#  - No ouptput Clock, 
#  - Low Crystal mode: >=8 MHz + start-up time: 16K CK cycles + 65 ms
# High Fuse HFUSE=0xD9
# - Enable Serial Programming & Downloading
# - Bootsize 2048 words (4096 bytes)
# Extended Fuse EFUSE=0xF9
# - Brown-out detection @ 3.5V
# - no Hardware Boot Vector (=boot at $0000)
# --------------------------------------


atmega328p: PART=m328p
atmega328p: TARGET = atmega328p
atmega328p: MCU= atmega328p
atmega328p: CFLAGS += 
atmega328p: AVR_FREQ = 16000000L 
atmega328p: LDSECTION  = --section-start=.text=0x0
atmega328p: $(PROGRAM)atmega328p.hex
atmega328p.o:  MCU=atmega328p
atmega328p.elf:  MCU=atmega328p
atmega328p.hex:  MCU=atmega328p
atmega328p.eep.hex:  MCU=atmega328p
atmega328p.era:  PART=m328p
atmega328p.bak:  PART=m328p
atmega328p.wfuse: PART=m328p
atmega328p.wfuse: LFUSE=0xFF
atmega328p.wfuse: HFUSE=0xD9
atmega328p.wfuse: EFUSE=0x05

atmega328p.rfuse: PART=m328p



LDSECTION  = --section-start=.text=0x0 

OPTIMIZE   = -O2

DEFS       = 
LIBS       =

CC         = avr-gcc


# AMFORTH VERSION TO USE
# 'code' for trunk and x.y for the releases (i.e 5.0)
#VERSION=5.0
VERSION=code
AMFORTH=
CORE=core

# directories
ATMEL=$(HOME)/atmel

# ------------------------
# PROGRAMMER CONFIGURATION
# ------------------------

PROGRAMMER=usbtiny
PORT=

AVRDUDE=avrdude
AVRDUDE_FLAGS= -c $(PROGRAMMER)

# ----------------
# ASSEMBLER TO USE
# ----------------

AS_INCLUDE=  -I $(CORE) -I $(CORE)/drivers -I $(CORE)/devices/$(MCU) -I words

#ASM=wine $(ATMEL)/avrasm2.exe
# flags Specific to avrasm2.exe
#AS_FLAGS=$(AS_INCLUDE) -fI -v0

#ASM=avra $(AS_FLAGS)

# Override is only needed by avr-lib build system.

override CFLAGS        = -v -Wall -g3 -gdwarf-2 -mmcu=$(MCU) -al
override LDFLAGS       = $(LDSECTION) -v -Map=map.txt 
#override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

ASM=avr-as $(AS_FLAGS) $(AS_INCLUDE)
LINK=avr-ld $(LDFLAGS)
#--------------------------
# Generic assemble patterns
#--------------------------

# Assemble the target
%.o : %.S
	@echo "Producing object files for ATMEL $*" 
#	@$(ASM) $(AS_FLAGS) -I $(CORE)/devices/$(MCU) -e $*.eep.hex -m $*.map -l $*.lst $<
	$(ASM) $(CFLAGS) -o $@ $^ > asout.txt

# link the target
%.elf : %.o
	@echo "Producing elf files for ATMEL $*"
	$(LINK) -mavr5 -o $@ $^
	avr-objdump -h -x -D -S $@ > a.lst

# make hex target
%.hex : %.elf
	@echo "Producing hex files for ATMEL $*"
	avr-objcopy -O ihex -R .eeprom -R .data $^ $@
	sed -i '/FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF/d' atmega328p.hex
	sed -i '/00000000000000000000000000000000/d' atmega328p.hex

# make eep target
%.eep.hex : %.elf
	@echo "Producing eep.hex files for ATMEL $*"
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $^ $*.eep.hex

# Flash the target
% : %.hex %.eep.hex
	@echo "Uploading Hexfiles to ATMEL $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -e -U flash:w:$*.hex:i 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U eeprom:w:$*.eep.hex:i

# Set the fuse bits
%.wfuse :
	@echo "Setting fuses to Arduino $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U efuse:w:$(EFUSE):m -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

# Set the fuse bits
%.rfuse :
	@echo "Setting fuses to Arduino $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U efuse:r:-:h -U hfuse:r:-:h -U lfuse:r:-:h

# Erase the whole MCU
%.era :
	@echo "Erasing entire Arduino $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -e

# Clear assembled & auxilars files
%.clr:
	@echo "Cleaning all aux files" 
	@rm -f $*.hex ; rm -f $*.eep.hex ; rm -f $*.lst ; rm -f $*.map ; rm -f $*.cof ; rm -f $*.obj

# Backup arduino Flash & EEPROM files
%.bak:
	@echo "Backup Flash & EEPRON from Arduino $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U flash:r:$*.hex.bak:i -U eeprom:r:$*.eep.hex.bak:i

# ----------------------------------------------------------

GENERIC_DEPENDECIES=*.inc words/*.S $(CORE)/*.S $(CORE)/drivers/*.S

# Assemble all targets is the default action

TARGET = #atmega328p.hex

%.S: MCU=atmega328p

default: $(TARGET)

$(TARGET) :  $(GENERIC_DEPENDENCIES)  $(CORE)/devices/*/*.S $(CORE)/devices/*/*.inc


# Cleans everything
clean:
	rm -f *.hex ; rm -f *.eep.hex ; rm -f *.lst ; rm -f *.map ; rm -f *.cof ; rm -f *.o

# All other rules are target specific and must be typed one by one
# as shown in the top.

