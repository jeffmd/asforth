; Partname:  ATmega328P

.include "m328Pdef.inc"

.pushsection .noinit
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
	 jmp ISREND ; External Interrupt Request 0
.org 0x08
	 jmp ISREND ; External Interrupt Request 1
.org 0x0c
	 jmp ISREND ; Pin Change Interrupt Request 0
.org 0x10
	 jmp ISREND ; Pin Change Interrupt Request 1
.org 0x14
	 jmp ISREND ; Pin Change Interrupt Request 2
.org 0x18
	 jmp ISREND ; Watchdog Time-out Interrupt
.org 0x1c
	 jmp ISREND ; Timer/Counter2 Compare Match A
.org 0x20
	 jmp ISREND ; Timer/Counter2 Compare Match B
.org 0x24
	 jmp ISREND ; Timer/Counter2 Overflow
.org 0x28
	 jmp ISREND ; Timer/Counter1 Capture Event
.org 0x2c
	 jmp ISREND ; Timer/Counter1 Compare Match A
.org 0x30
	 jmp ISREND ; Timer/Counter1 Compare Match B
.org 0x34
	 jmp ISREND ; Timer/Counter1 Overflow
.org 0x38
	 jmp ISREND ; TimerCounter0 Compare Match A
.org 0x3c
	 jmp ISREND ; TimerCounter0 Compare Match B
.org 0x40
	 jmp ISREND ; Timer/Couner0 Overflow
.org 0x44
	 jmp ISREND ; SPI Serial Transfer Complete
.org 0x48
	 jmp ISREND ; USART Rx Complete
.org 0x4c
	 jmp ISREND ; USART, Data Register Empty
.org 0x50
	 jmp ISREND ; USART Tx Complete
.org 0x54
	 jmp ISREND ; ADC Conversion Complete
.org 0x58
	 jmp ISREND ; EEPROM Ready
.org 0x5c
	 jmp ISREND ; Analog Comparator
.org 0x60
	 jmp ISREND ; Two-wire Serial Interface
.org 0x64
	 jmp ISREND ; Store Program Memory Read
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
