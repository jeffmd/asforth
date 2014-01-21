; Partname:  ATmega328P

.include "m328Pdef.inc"

.pushsection .data
.org 0xa0 ; SRAM_START
.popsection

.equ CELLSIZE,  2

.macro writeflashcell
	lsl ZL
	rol ZH
.endm

.set WANT_USART0,  0
.set WANT_TWI,  0
.set WANT_TIMER_COUNTER_1,  0
.set WANT_TIMER_COUNTER_2,  0
.set WANT_AD_CONVERTER,  0
.set WANT_ANALOG_COMPARATOR,  0
.set WANT_PORTB,  0
.set WANT_PORTC,  0
.set WANT_PORTD,  0
.set WANT_TIMER_COUNTER_0,  0
.set WANT_EXTERNAL_INTERRUPT,  0
.set WANT_SPI,  0
.set WANT_WATCHDOG,  0
.set WANT_CPU,  0
.set WANT_EEPROM,  0
.equ intvecsize,  2 ; please verify; flash size: 32768 bytes
.equ pclen,  2 ; please verify

int_vectors:
 	 jmp COLD ; reset Handler
.org 0x04
	 rcall isr ; External Interrupt Request 0
.org 0x08
	 rcall isr ; External Interrupt Request 1
.org 0x0c
	 rcall isr ; Pin Change Interrupt Request 0
.org 0x10
	 rcall isr ; Pin Change Interrupt Request 1
.org 0x14
	 rcall isr ; Pin Change Interrupt Request 2
.org 0x18
	 rcall isr ; Watchdog Time-out Interrupt
.org 0x1c
	 rcall isr ; Timer/Counter2 Compare Match A
.org 0x20
	 rcall isr ; Timer/Counter2 Compare Match B
.org 0x24
	 rcall isr ; Timer/Counter2 Overflow
.org 0x28
	 rcall isr ; Timer/Counter1 Capture Event
.org 0x2c
	 rcall isr ; Timer/Counter1 Compare Match A
.org 0x30
	 rcall isr ; Timer/Counter1 Compare Match B
.org 0x34
	 rcall isr ; Timer/Counter1 Overflow
.org 0x38
	 rcall isr ; TimerCounter0 Compare Match A
.org 0x3c
	 rcall isr ; TimerCounter0 Compare Match B
.org 0x40
	 rcall isr ; Timer/Couner0 Overflow
.org 0x44
	 rcall isr ; SPI Serial Transfer Complete
.org 0x48
	 rcall isr ; USART Rx Complete
.org 0x4c
	 rcall isr ; USART, Data Register Empty
.org 0x50
	 rcall isr ; USART Tx Complete
.org 0x54
	 rcall isr ; ADC Conversion Complete
.org 0x58
	 rcall isr ; EEPROM Ready
.org 0x5c
	 rcall isr ; Analog Comparator
.org 0x60
	 rcall isr ; Two-wire Serial Interface
.org 0x64
	 rcall isr ; Store Program Memory Read
.equ INTVECTORS,  26

; compatability layer (maybe empty)
.equ SPMEN,  SELFPRGEN

; controller data area, environment query mcu-info
mcu_info:
mcu_ramsize:
	.word 2048
mcu_eepromsize:
	.word 1024
mcu_maxdp:
	.word 28672 
mcu_numints:
	.word 26
mcu_name:
	ascii_ 10, "ATmega328P"

.set codestart, .
