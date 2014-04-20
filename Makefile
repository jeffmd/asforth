# Simple makefile for building  
# asforth for atmega328p target

# Examples of usage for atmega328p:
#
# 1) Assemble the whole flash and eemprom files
#     make atmega328p.hex
#
# 2) Backup the current flash & eeprom values 
#     make atmega328p.bak
#
# 3) Erase the whole MCU Flash
#    make atmega328p.era
#
# 4) Upload the new firmware using the hex file generated
#    make atmega328p
#
# 5) Set the appropiate MCU fuses
#    make atmega328p.fuse
#
# 6) Clear files (except backup)
#    make atmega328p.clr


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
# Example fuse settings for atmega328p
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


atmega328p : PART=m328p
atmega328p : TARGET = atmega328p
atmega328p : MCU= atmega328p
atmega328p : AVR_FREQ = 16000000L 
atmega328p : LDSECTION  = --section-start=.text=0x0
atmega328p : $(PROGRAM)atmega328p.hex
atmega328p.o :  MCU=atmega328p
atmega328p.elf :  MCU=atmega328p
atmega328p.hex :  MCU=atmega328p
atmega328p.eep.hex :  MCU=atmega328p
atmega328p.era :  PART=m328p
atmega328p.bak :  PART=m328p
atmega328p.wfuse : PART=m328p
atmega328p.wfuse : LFUSE=0xFF
atmega328p.wfuse : HFUSE=0xD9
atmega328p.wfuse : EFUSE=0x05
atmega328p.rfuse: PART=m328p


# ASFORTH VERSION TO USE
# 'code' for trunk and x.y for the releases (i.e 1.0)
#VERSION=1.1
VERSION=code
ASFORTH=
CORE=core

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

# Override is only needed by avr-lib build system.
LDSECTION  = --section-start=.text=0x0 

override ASFLAGS  = -v -Wall -g3 -gdwarf-2 -mmcu=$(MCU) -al
override LDFLAGS  = $(LDSECTION) -v -Map=map.txt -mavr5 

ASM = avr-as $(ASFLAGS) $(AS_INCLUDE)
LINK = avr-ld $(LDFLAGS)
	
INCS = $(CORE)/*.inc $(CORE)/*.S $(CORE)/drivers/*.S $(CORE)/devices/atmega328p/*.S

# Assemble the target
atmega328p.o : atmega328p.S $(INCS)
	@echo "Producing object files for ATMEL $*" 
	$(ASM) -o $@ atmega328p.S > asout.txt

	
# link the target
%.elf : %.o
	@echo "Producing elf files for ATMEL $*"
	$(LINK) -o $@ $^
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
	@echo "Setting fuses to ATMEL $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U efuse:w:$(EFUSE):m -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

# read the fuse bits
%.rfuse :
	@echo "Setting fuses to ATMEL $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U efuse:r:-:h -U hfuse:r:-:h -U lfuse:r:-:h

# Erase the whole MCU
%.era :
	@echo "Erasing entire ATMEL $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -e

# Clear assembled & auxilars files
%.clr:
	@echo "Cleaning all aux files" 
	@rm -f $*.hex ; rm -f $*.eep.hex ; rm -f $*.lst ; rm -f $*.map ; rm -f *.o

# Backup arduino Flash & EEPROM files
%.bak:
	@echo "Backup Flash & EEPRON from atmega328p $*" 
	$(AVRDUDE) $(AVRDUDE_FLAGS) -p $(PART) -U flash:r:$*.hex.bak:i -U eeprom:r:$*.eep.hex.bak:i

# ----------------------------------------------------------


# Cleans everything
clean :
	rm -f *.hex ; rm -f *.eep.hex ; rm -f *.lst ; rm -f *.map ; rm -f *.o

# All other rules are target specific and must be typed one by one
# as shown in the top.
