;;;; avr atmega simple forth based on amforth
;;;;
;;;; GPL V2 (only)

; lower part of the dictionary
; this dictionary area contains optional/application words
; they may be moved to the core dictionary if needed

.include "dict_interrupt.inc"
.include "dict_minimum.inc"
.include "dict_asm.inc"

;**********
.Set DPSTART, .

.org NRWW_START_ADDR * 2, 0xff
.include "dict_core.inc"

.equiv flashlast, .
;************ FIXME ************
; how to calculate how much upper flash has been used?
;.if (flashlast < (FLASHEND * 2))
;  .error "*** Flash size exceeded, please edit your dict_appl_core file to use less space! Aborting."
;.endif

.pushsection .data
; define a label for the 1st free ram address
HERESTART:
.popsection

.pushsection .eeprom
; how to setup eeprom section in avr-as
.include "eeprom.inc"
; 1st free address in EEPROM.
EDPSTART:
.popsection
