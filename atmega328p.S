.section .progmem
; some defaults

.set WANT_ISR_TX, 0
.set WANT_ISR_RX, 0

; Dictionary setup
.set VE_HEAD, 0x0000

.include "user.inc"
.include "macros.S"
.include "device.S"

.equ TIBSIZE, 0x50   ; 80 characters is one line...

; cpu clock in hertz
.equ F_CPU, 16000000
; baud rate of terminal
.equ BAUD, 38400
.include "drivers/usart_0.S"

.Set rstackstart, RAMEND
.Set stackstart, RAMEND - 50

.include "asforth.S"
