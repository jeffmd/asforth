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
	 jmp isr ; External Interrupt Request 0
.org 0x08
	 rcall isr ; External Interrupt Request 1
.org 0x0c
	 jmp isr ; Pin Change Interrupt Request 0
.org 0x10
	 jmp isr ; Pin Change Interrupt Request 1
.org 0x14
	 jmp isr ; Pin Change Interrupt Request 2
.org 0x18
	 jmp isr ; Watchdog Time-out Interrupt
.org 0x1c
	 jmp isr ; Timer/Counter2 Compare Match A
.org 0x20
	 jmp isr ; Timer/Counter2 Compare Match B
.org 0x24
	 jmp isr ; Timer/Counter2 Overflow
.org 0x28
	 jmp isr ; Timer/Counter1 Capture Event
.org 0x2c
	 jmp isr ; Timer/Counter1 Compare Match A
.org 0x30
	 jmp isr ; Timer/Counter1 Compare Match B
.org 0x34
	 jmp isr ; Timer/Counter1 Overflow
.org 0x38
	 jmp isr ; TimerCounter0 Compare Match A
.org 0x3c
	 jmp isr ; TimerCounter0 Compare Match B
.org 0x40
	 jmp isr ; Timer/Couner0 Overflow
.org 0x44
	 jmp isr ; SPI Serial Transfer Complete
.org 0x48
	 jmp isr ; USART Rx Complete
.org 0x4c
	 jmp isr ; USART, Data Register Empty
.org 0x50
	 jmp isr ; USART Tx Complete
.org 0x54
	 jmp isr ; ADC Conversion Complete
.org 0x58
	 jmp isr ; EEPROM Ready
.org 0x5c
	 jmp isr ; Analog Comparator
.org 0x60
	 jmp isr ; Two-wire Serial Interface
.org 0x64
	 jmp isr ; Store Program Memory Read
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
