
; some defaults

.set WANT_ISR_TX, 0
.set WANT_ISR_RX, 0

; Dictionary setup
.set VE_HEAD, 0x0000


.include "user.inc"

  ; registers 0, 1 not used except in interrupt service routines
  isrZL = 0
  isrZH = 1

  ; register 2, 3 not used except in interrupt service routines
  isrXL = 2 
  isrXH = 3
  
  upl = 4 ; ram user area 
  uph = 5

  ; address A
  al = 6
  ah = 7
  ; address B
  bl = 8
  bh = 9
  
  ; registars 10, 11, 12, 13 not used except in interrupt service routines
  isrt0 = 10
  isrt1 = 11
  isrt2 = 12
  isrt3 = 13
  
  temp4 = 14
  temp5 = 15

  temp0 = 16
  temp1 = 17
  temp2 = 18
  temp3 = 19

  temp6 = 20
  temp7 = 21

; header flags
.set HFLAGS, 0xFF00
; low byte is word count : 0 - 255
; high byte is flags for word
; enable inlining of word
; bit 0 : enable inlinning
.set INLINE_OPT_FB, 0
.set INLINE_OPT, 0x01
; bit 2 : dissable tail call optimization
; disable tail call optimization : & with word count flag ie: HFLAGS & DIS_CALL_OPT | 04
.set DIS_CALL_OPT, 0x04

; bit 3 : disable interpret execution ie compile only
.set COMPILE_ONLY_FB, 3
; set immediate word as compile time only execution
; disable interpret execution : & with word count flag ie: 0xFF04 & COMPILE_ONLY
.set COMPILE_ONLY, 0x08

; bit 7 : immediate execution when in compile state
; enable immediate execution : & with word count flag ie: 0xFF04 & IMMEDIATE_EN
.set IMMEDIATE_EN_FB, 7
.set IMMEDIATE_EN, 0x80

; forth flags
; bit 0 : rcall was last instruction
.set LAST_RCALL_FB, 0
; bit 1 : call was last instruction
.set LAST_CALL_FB, 1
; if bit 0 and 1 are cleared then last instruction was an inline or tail call optimization was disabled by word
; bit 2 : disable tail call optimization
.set DIS_CALL_OPT_FB, 2

; forth flags register
  fflags = 22
  
; always has zero / cleared
  zerol = 23
  
  ; parameter top of stack [TOS] register
  tosl = 24
  tosh = 25


  ; XL (26), XH (27) general purpose addressing
  ; YL (28), YH (29) is to hold the Forth VM Parameter/Data stack pointer
  ; ZL (30), ZH (31) are used as memory pointer
  ; SP is used to hold the Forth VM return stack pointer and is the hardware stack pointer for the MCU

; Inline macros
.macro dup_r_ ; put copy of tos on return stack
    push tosl
    push tosh
.endm

.macro r_drop_ ; drop TOR
    pop temp1
    pop temp0
.endm

.macro poptos ; load tos, (drop)
    ld tosl, Y+
    ld tosh, Y+
.endm

.macro drop_
    poptos
.endm

.macro pushtos ; save tos, (dup)
    st -Y, tosh
    st -Y, tosl
.endm

.macro dup_
    pushtos
.endm

.macro oneplus_
    adiw tosl, 1
.endm

.macro oneminus_
    sbiw tosl, 1
.endm

.macro twoplus_
    adiw tosl, 2
.endm

.macro twominus_
    sbiw tosl, 2
.endm

.macro twoslash_
    asr tosh
    ror tosl
.endm

.macro twostar_
    lsl tosl
    rol tosh
.endm

.macro nip_
    adiw YL, 2
.endm

.macro dnip_
    adiw YL, 4
.endm

.macro not_
    com tosl
    com tosh
.endm

.macro zerosense_
    sbiw tosl, 0
    poptos
.endm

.macro dupzerosense_
    sbiw tosl, 0
.endm

.macro zerotos_
    clr tosl
    clr tosh
.endm

.macro anditos_ val
    andi tosh, hi8(\val)
    andi tosl, lo8(\val)
.endm

.macro oritos_ val
    ori tosh, hi8(\val)
    ori tosl, lo8(\val)
.endm

.macro doliteral_ val
    pushtos
    ldi tosl, lo8(\val) 
    ldi tosh, hi8(\val) 
.endm

.macro zerohigh_
    clr tosh
.endm

.macro zerolow_
    clr tosl
.endm

.macro swapnib_
    swap tosl
.endm

.macro niptoa_
    ld al, Y+
    ld ah, Y+
.endm

.macro niptob_
    ld bl, Y+
    ld bh, Y+
.endm


.macro aplus_
    movw al, ZL
.endm


.macro copytoa_
    movw al, tosl
.endm

.macro copytob_
    movw bl, tosl
.endm

.macro in_ val, prt
.if (\prt < 0x40)
  in \val, \prt
.else
  lds \val, \prt
.endif
.endm

.macro out_ prt, val
.if (\prt < 0x40)
  out \prt, \val
.else
  sts \prt, \val
.endif
.endm

.macro sbi_
.if (@0 < $40)
  sbi @0,@1
.else
  in_ @2,@0
  ori @2,exp2(@1)
  out_ @0,@2
.endif
.endm

.macro cbi_
.if (@0 < $40)
  cbi @0,@1
.else
  in_ @2,@0
  andi @2,~(exp2(@1))
  out_ @0,@2
.endif
.endm

.macro addr_ sym
  .word pm(\sym)
.endm

.macro ascii_ sz, str
  .word \sz
  .ascii "\str"
  .align 1
.endm

.macro forthword_ lbl, flags, sz, str 
VE_\lbl:
  ascii_ ((HFLAGS & ~(\flags<<8)) | \sz), "\str"
  addr_ VE_HEAD
  .set VE_HEAD, VE_\lbl
\lbl:
.endm


;               F_CPU
;    µsec   16000000   14745600    8000000  1000000
;    1            16      14,74          8        1
;    10          160     147,45         80       10
;    100        1600    1474,56        800      100
;    1000      16000   14745,6        8000     1000
;
; cycles, µsec * f_cpu / 1e6
; n_loops=cycles/5
;
;     cycles already used will be subtracted from the delay
;     the waittime resolution is 1 cycle (delay from exact to +1 cycle)
;     the maximum delay at 20MHz (50ns/clock) is 38350ns
;     waitcount register must specify an immediate register
;
; busy waits a specfied amount of microseconds
.macro   delay micros
      .set cycles, ( ( \micros * F_CPU ) / 1000000 )
      .if (cycles > ( 256 * 255 * 4 + 2))
        .error "MACRO delay - too many cycles to burn"
      .else
        .if (cycles > 6)
          .set  loop_cycles, (cycles / 4)      
          ldi   zl,low(loop_cycles)
          ldi   zh,high(loop_cycles)
delay_loop:
          sbiw  Z, 1
          brne  delay_loop
          .set  cycles, (cycles - (loop_cycles * 4))
        .endif
        .if (cycles > 0)
          .if   (cycles & 4)
            rjmp  pc+1
            rjmp  pc+1
          .endif
          .if   (cycles & 2)
            rjmp  pc+1
          .endif
          .if   (cycles & 1)
            nop
          .endif
        .endif
      .endif
.endm
