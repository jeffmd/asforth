; this part of the dictionay has to fit into the nrww flash

; ( -- ) (R: nest-sys -- )
; Compiler
; exit the word
;forthword_ EXIT, 0x8004, "exit"
;    r_drop_
;    ret

; ( xt -- ) 
; System
; execute XT
forthword_ EXEC, 0x8004, "exec"
    movw ZL, tosl
    poptos
    ijmp

;;;;;;;;;;;; Parameter Stack ;;;;;;;;;;;;;
; ( n -- n n ) 
; Stack
; duplicate TOS
forthword_ DUP, 0x8203, "dup"
    pushtos
    ret

; ( n1 -- [ n1 n1 ] | 0) 
; Stack
; duplicate TOS if non-zero
forthword_ QDUP, 0x8004, "?dup"
    sbiw tosl, 0
    breq PFA_QDUP1
    pushtos
PFA_QDUP1:
    ret

; ( n1 n2 -- n2 n1) 
; Stack
; swaps the two top level stack cells
forthword_ SWAP, 0x8004, "swap"
    movw temp0, tosl
    poptos
    st -Y, temp1
    st -Y, temp0
    ret

; ( x1 x2 -- x1 x2 x1 ) 
; Stack
; Place a copy of x1 on top of the stack
forthword_ OVER, 0x8004, "over"
    pushtos
    ldd tosl, Y+2
    ldd tosh, Y+3
    ret

; ( x1 x2 -- x1 x2 x1 x2) 
; Stack
; Place a copy of x1 and x2 on top of the stack
forthword_ TWOOVER, 0x8005, "2over"
    rcall OVER
    rjmp OVER

; ( n -- ) 
; Stack
; drop TOS
forthword_ DROP, 0x8204, "drop"
    drop_
    ret

; ( n n -- ) 
; Stack
; drop TOS twice
forthword_ TWODROP, 0x8005, "2drop"
    adiw YL, 2
    drop_
    ret

; ( n1 n2 -- n2) 
; Stack
; drop NOS, one before TOS.
; same as SWAP DROP
forthword_ NIP, 0x8103, "nip"
    nip_
    ret

; ( n1 n2 -- n2 n1 n2) 
; Stack
; tuck TOS before NOS, same as SWAP OVER
forthword_ TUCK, 0x8004, "tuck"
    ld temp0, y+
    ld temp1, y+
    st -y, tosh 
    st -y, tosl
    st -y, temp1 
    st -y, temp0
    ret

; ( n1 n2 n3 -- n3 n2 n1) 
; Stack
; exchange TOS and stack cell before NOS, same as SWAP ROT
forthword_ FLIP, 0x8004, "flip"
    ldd temp0, y+2
    ldd temp1, y+3
    std y+3, tosh 
    std y+2, tosl
    movw tosl, temp0 
    ret

; ( n1 n2 n3 -- n2 n3 n1) 
; Stack
; rotate the three top level cells
forthword_ ROT, 0x8003, "rot"
    movw temp0, tosl
    ld temp2, Y+
    ld temp3, Y+ 
    poptos
    st -Y, temp3
    st -Y, temp2
    st -Y, temp1
    st -Y, temp0
    ret

; ( n1 n2 n3 -- n3 n1 n2) 
; Stack
; reverse rotate the three top level cells
; equivalent to rotate twice but faster
forthword_ RROT, 0x8004, "rrot"
    ld temp0, Y+
    ld temp1, Y+
    ld temp2, Y+
    ld temp3, Y+
    st -Y, tosh
    st -Y, tosl
    st -Y, temp3
    st -Y, temp2
    movw tosl, temp0
    ret

;;;;;;;;;;;; Return Stack ;;;;;;;;;;;;;
; ( -- n ) (R: n --)
; Stack
; move TOR to TOS
; call only
forthword_ R_FROM, 0x8002 | DIS_CALL_OPT, "r>"
    pop ZH
    pop ZL
    pushtos
    pop tosh
    pop tosl
    ijmp

; ( -- ) (R: n -- )
; Stack
; Drop TOR
forthword_ R_DROP, 0x8206, "r>drop"
    r_drop_
    ret

; ( -- a-addr )
; Stack
; pop return onto data stack and exit caller
; must be called, do not use jmp
; used if data in program mem and need an address to it
forthword_ POPRET, 0x8006 | DIS_CALL_OPT, "popret"
    pushtos
    pop tosh
    pop tosl
    ret


; ( n -- n ) (R: -- n )
; Stack
; copy TOS to TOR
forthword_ DUP_R, 0x8205, "dup>r"
    dup_r_
    ret

; ( n -- ) (R: -- n)
; Stack
; move TOS to TOR
; call only
forthword_ TO_R, 0x8002 | DIS_CALL_OPT, ">r"
    pop ZH
    pop ZL
    dup_r_
    poptos
    ijmp

; ( -- n) (R: n -- n )
; Stack
; use TOR as address to fetch content from ram
forthword_ R_FETCH, 0x8002, "r@"
    in ZL, SPL
    in ZH, SPH
    pushtos
    ldd tosh, Z+3
    ldd tosl, Z+4
    ret

; ( -- n) (R: n -- n+1 )
; Stack
; fetch content of TOR and then increment TOR by 1
; call only
forthword_ R_FETCHPLUS, 0x8004 | DIS_CALL_OPT, "r>r+"
    pop ZH
    pop ZL
    pushtos
    pop tosh
    pop tosl
    adiw tosl, 0x01
    push tosl
    push tosh
    sbiw tosl, 0x01
    ijmp


;;;;;;;;;;;;; Operators ;;;;;;;;;;;
; ( n1 n2 -- flag) 
; Compare
; true if n1 is not equal to n2
forthword_ NOTEQUAL, 0x8002, "<>"
    rjmp XOR

; ( n -- flag ) 
; Compare
; compare with 0 (zero)
forthword_ EQUALZERO, 0x8002, "0="
    or tosh, tosl
    rjmp PFA_EQUALDONE

; ( n1 n2 -- flag ) 
; Compare
; compares two values for equality

forthword_ EQUAL, 0x8001, "="
    ld temp2, Y+
    ld temp3, Y+
    cp tosl, temp2
    cpc tosh, temp3
PFA_EQUALDONE:
    brne PFA_ZERO1
    rjmp PFA_TRUE1

; ( n1 n2 -- flag) 
; Compare
; true if n1 is less than n2
forthword_ LESS, 0x8001, "<"
    ld temp2, Y+
    ld temp3, Y+
    cp temp2, tosl
    cpc temp3, tosh
PFA_LESSDONE:
    brge PFA_ZERO1
    rjmp PFA_TRUE1

; ( n1 n2 -- flag ) 
; Compare
; flag is true if n1 is greater than n2
forthword_ GREATER, 0x8001, ">"
    ld temp2, Y+
    ld temp3, Y+
    cp temp2, tosl
    cpc temp3, tosh
PFA_GREATERDONE:
    brlt PFA_ZERO1
    brbs 1, PFA_ZERO1
    rjmp PFA_TRUE1

; ( n1 -- flag) 
; Compare
; compare with zero
forthword_ LESSZERO, 0x8002, "0<"
    cp tosl, zerol
    cpc tosh, zerol
    rjmp PFA_LESSDONE

; ( u1 u2 -- flag ) 
; Compare
; true if u1 > u2 (unsigned)
forthword_ UGREATER, 0x8002, "u>"
    ld temp2, Y+
    ld temp3, Y+
    cp temp2, tosl
    cpc temp3, tosh
    brlo PFA_ZERO1
    brbs 1, PFA_ZERO1
    rjmp PFA_TRUE1

; ( n1 -- flag ) 
; Compare
; true if n1 is greater than 0
forthword_ GREATERZERO, 0x8002, "0>"
    cp tosl, zerol
    cpc tosh, zerol
    rjmp PFA_GREATERDONE

; ( -- 0 ) 
; Arithmetics
; set TOS to 0
;forthword_ ZEROSET, 0x8102, "0:"
;    zerotos_
;    ret

; ( -- 0 ) 
; Arithmetics
; place a value 0 on TOS
forthword_ ZERO, 0x8001, "0"
    pushtos
PFA_ZERO1:
    zerotos_
    ret

; ( -- 1 ) 
; Arithmetics
; place a value 1 on TOS
forthword_ ONE, 0x8001, "1"
    pushtos
PFA_ONE1:
    ldi tosh, 0
    ldi tosl, 1
    ret

; ( -- 2 ) 
; Arithmetics
; place a value 2 on TOS
forthword_ TWO, 0x8001, "2"
    pushtos
PFA_TWO1:
    ldi tosh, 0
    ldi tosl, 2
    ret

; ( -- 10 ) 
; Arithmetics
; place a value 10 on TOS
forthword_ TEN, 0x8002, "10"
    pushtos
PFA_TEN1:
    ldi tosh, 0
    ldi tosl, 10
    ret

; ( -- -1 ) 
; Arithmetics
; leaves the value -1 (true) on TOS
forthword_ TRUE, 0x8004, "true"
    pushtos
PFA_TRUE1:
    ser tosl
    ser tosh
    ret

; ( n -- ) 
; Compare
; test TOS if zero
forthword_ ZEROSENSE, 0x8302, "0?"
    zerosense_
    ret

; ( n -- n) 
; Compare
; test TOS if zero, does not consume TOS
forthword_ QZEROSENSE, 0x8103, "?0?"
    dupzerosense_
    ret

; ( u1 u2 -- flag) 
; Compare
; true if u1 < u2 (unsigned)
forthword_ ULESS, 0x8002, "u<"
    ld temp2, Y+
    ld temp3, Y+
    cp tosl, temp2
    cpc tosh, temp3
    brlo PFA_ZERO1
    brbs 1, PFA_ZERO1
    rjmp PFA_TRUE1


; ( n1|u1 n2|u2 -- n3|u3 ) 
; Arithmetics
; subtract n2 from n1
forthword_ MINUS, 0x8001, "-"
    ld temp0, Y+
    ld temp1, Y+
    sub temp0, tosl
    sbc temp1, tosh
    movw tosl, temp0
    ret

; ( n1 n2 -- n3) 
; Arithmetics
; add n1 and n2
forthword_ PLUS, 0x8001, "+"
    ld temp0, Y+
    ld temp1, Y+
    add tosl, temp0
    adc tosh, temp1
    ret


; ( n1 -- n2) 
; Arithmetics
; 1-complement of TOS
forthword_ INVERT, 0x8206, "invert"
    invert_
    ret

; ( ddddkkkk -- kkkkdddd )
; Arithmetics
; swap nibbles in lower half of TOS
forthword_ SWAPNIB, 0x8105, "swnib"
    swapnib_
    ret

; ( n1 -- n2 ) 
; Arithmetics
; arithmetic shift right
forthword_ TWOSLASH, 0x8202, "2/"
    twoslash_
    ret

; ( n1 -- n2 ) 
; Arithmetics
; arithmetic shift left, filling with zero
forthword_ TWOSTAR, 0x8202, "2*"
    twostar_
    ret

; ( n1 n2 -- n3 ) 
; Logic
; bitwise and

forthword_ AND, 0x8003, "and"
    ld temp0, Y+
    ld temp1, Y+
    and tosl, temp0
    and tosh, temp1
    ret

; ( n1 n2 -- n3 ) 
; Logic
; logical or
forthword_ OR, 0x8002, "or"
    ld temp0, Y+
    ld temp1, Y+
    or tosl, temp0
    or tosh, temp1
    ret

; ( n1 n2 -- n3) 
; Logic
; exclusive or
forthword_ XOR, 0x8003, "xor"
    ld temp0, Y+
    ld temp1, Y+
    eor tosl, temp0
    eor tosh, temp1
    ret


; ( n1|u1 -- n2|u2 ) 
; Arithmetics
; optimized increment

forthword_ ONEPLUS, 0x8102, "1+"
    oneplus_
    ret

; (S: n1 -- n2 )
; Arithmetics
; optimized decrement

forthword_ ONEMINUS, 0x8102, "1-"
    oneminus_
    ret

; ( n1|u1 -- n2|u2 ) 
; Arithmetics
; optimized two increment
forthword_ TWOPLUS, 0x8102, "2+"
    twoplus_
    ret

; (S: n1 -- n2 )
; Arithmetics
; optimized decrement
forthword_ TWOMINUS, 0x8102, "2-"
    twominus_
    ret

; ( n1 -- n2/2 ) 
; Arithmetics
; convert byte length to word align length
; round number up to even value then right shift

forthword_ WALIGN, 0x8002, "w_"
    oneplus_
	andi tosl, 0xfe
    twoslash_
    ret


; ( n1 n2 -- n3) 
; Arithmetics
; logically shift n1 left n2 times
forthword_ LSHIFT, 0x8002, "<<"
    movw ZL, tosl
    poptos
PFA_LSHIFT1:
    sbiw ZL, 1
    brmi PFA_LSHIFT2
    lsl tosl
    rol tosh
    rjmp PFA_LSHIFT1
PFA_LSHIFT2:
    ret

; ( n1 n2 -- n3 ) 
; Arithmetics
; shift n1 n2-times logically right
forthword_ RSHIFT, 0x8002, ">>"
    movw ZL, tosl
    poptos
PFA_RSHIFT1:
    sbiw ZL, 1
    brmi PFA_RSHIFT2
    lsr tosh
    ror tosl
    rjmp PFA_RSHIFT1
PFA_RSHIFT2:
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; (  -- n) 
; Stack
; current return stack pointer address
; must be called, don't use jmp to optimize
forthword_ RP_FETCH, 0x8003 | DIS_CALL_OPT, "rp@"
    pushtos
    in tosl, SPL
    in tosh, SPH
    ; don't include return address
    twoplus_
    ret

; ( addr  -- ) (R: -- x*y)
; Stack
; set return stack pointer
; must be called
forthword_ RP_STORE, 0x8003 | DIS_CALL_OPT, "rp!"
    in temp2, SREG
    cli
    ; keep the return address
    pop ZH
    pop ZL
    out SPL, tosl
    out SPH, tosh
    out SREG, temp2
    poptos
    ijmp

; (  -- addr ) 
; Stack
; current data stack pointer
forthword_ SP_FETCH, 0x8003, "sp@"
    pushtos
    movw tosl, YL
    ret

; ( addr -- i*x) 
; Stack
; set data stack pointer to addr
forthword_ SP_STORE, 0x8003, "sp!"
    movw YL, tosl
    poptos
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;

; (addr-from addr-to n -- ) 
; Memory
; copy data in RAM from higher to lower addresses.

forthword_ CMOVE_G, 0x8002, "c>"
    ld ZL, Y+
    ld ZH, Y+ ; addr-to
    ld XL, Y+
    ld XH, Y+ ; addr-from
    mov temp0, tosh
    or temp0, tosl
    brbs 1, PFA_CMOVE_G1
    add ZL, tosl
    adc ZH, tosh
    add XL, tosl
    adc XH, tosh
PFA_CMOVE_G2:
    ld temp1, -X
    st -Z, temp1
    oneminus_
    brbc 1, PFA_CMOVE_G2
PFA_CMOVE_G1:
    poptos
    ret

; ( n1 -- n2 ) 
; Arithmetics
; exchange the bytes of the TOS

forthword_ BYTESWAP, 0x8002, "><"
    mov temp0, tosh
    mov tosh, tosl
    mov tosl, temp0
    ret

; ( n1 -- n2 ) 
; Arithmetics
; clear the high byte of the TOS

forthword_ ZEROHIGH, 0x8102, "0~"
    zerohigh_
    ret

; ( n1 -- n2 ) 
; Arithmetics
; clear the low byte of the TOS
forthword_ ZEROLOW, 0x8102, "~0"
    zerolow_
    ret

; ( -- addr ) 
; System Variable
; get user area pointer
forthword_ UP_FETCH, 0x8003, "up@"
    pushtos
    movw tosl, upl
    ret

; ( addr -- ) 
; System Variable
; set user area pointer
forthword_ UP_STORE, 0x8003, "up!"
    movw upl, tosl
    poptos
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- n1 )
; System
; runtime of literal
;forthword_ DOLITERAL, 0x8009, "(literal)"
;DOLITERAL:
;    pushtos
;    pop ZH
;    pop ZL
;    readflashcell tosl,tosh
;    ror ZH
;    ror ZL
;    ijmp    ; (z)

; ( -- addr )
; System
; puts content of parameter field (1 cell) to TOS
;forthword_ DOVARIABLE, 0x800a, "(variable)"
;DOVARIABLE:
;    pushtos
;    pop     ZH    ; pop return address - will not be returning here
;    pop     ZL
;    rjmp ZDFETCHI

; ( -- addr )
; System
; place data field address on TOS
;forthword_ DOCONSTANT, 0x800a, "(constant)"
;    pop ZH
;    pop ZL    
;    pushtos
;    movw tosl, ZL
;    oneplus_
;    adiw ZL, 1
;    ijmp
    
; ( -- addr )
; System
; runtime part of user
; get the address of the user var which is located at the return address of the caller
;forthword_ DOUSER, 0x8006, "(user)"
DOUSER:
    pushtos
    pop ZH
    pop ZL
    rcall ZDFETCHI
    ; calculate user address
    add tosl, upl
    adc tosh, uph
    ret

; ( -- n )
; System
; runtime of value
; must use call
forthword_ DOVALUE, 0x8007 | DIS_CALL_OPT, "(value)"
; must be called, don't use jmp
    ; Top of return stack has return address of caller
    ; return address is actually the address of the value
    ; won't be returning to this address
    rcall R_FROM        ; ( raddr ) (R: raddr -- )
    rjmp DEFX           ; ( n )

; ( addr -- n )
; System
; execute the defer fetch
forthword_ DEFX, 0x8004, "defx"
    dup_                ; ( addr addr )
    ; get address of defer
    rcall FETCHI        ; ( addr valaddr )
    rcall SWAP          ; ( valaddr addr )
    ; get the fetch Xt pointer which is one word after the data
    oneplus_
    rcall FETCHI        ; ( valaddr xtfetch )
    ; EXEC the fetch
    rjmp EXEC        ; ( n )


; ( n e-addr -- ) 
; Memory
; write n (2bytes) to eeprom address
forthword_ STOREE, 0x8002, "!e"
    movw ZL, tosl
    poptos
    in_ temp2, SREG
    cli
    rcall PFA_STOREE1
    adiw ZL,1

    mov tosl, tosh
    rcall PFA_STOREE1
    out_ SREG, temp2
    poptos
    ret
    
PFA_STOREE1:
    sbic EECR, EEPE
    rjmp PFA_STOREE1

PFA_STOREE2: ; estore_wait_low_spm:
    in_ temp0, SPMCSR
    sbrc temp0,SPMEN
    rjmp PFA_STOREE2

    out_ EEARH,ZH
    out_ EEARL,ZL
    out_ EEDR, tosl
    sbi EECR,EEMPE
    sbi EECR,EEPE

    ret

; ( e-addr - n) 
; Memory
; read 1 cell from eeprom
forthword_ FETCHE, 0x8002, "@e"
    in_ temp2, SREG
    cli
    movw ZL, tosl
    rcall FETCHE_READ
    in_ tosl, EEDR

    adiw ZL, 1

    rcall FETCHE_READ
    in_  tosh, EEDR
    out_ SREG, temp2
    adiw ZL, 1
    ret

FETCHE_READ:
    sbic EECR, EEPE
    rjmp FETCHE_READ

    out_ EEARH,ZH
    out_ EEARL,ZL

    sbi EECR,EERE
    ret


; ( n f-addr -- ) 
; Memory
; writes n to flash memory using assembly code (code to be placed in boot loader section)
forthword_ STOREI, 0x8002, "!i"
  ; store status register
  in temp1,SREG
  push temp1
  cli

  movw temp2, tosl ; save the (word) address
  poptos          ; get the new value for the flash cell
  ; save parameter stack
  push YL
  push YH
  rcall DO_STOREI_atmega
  ; restore parameter stack
  pop YH
  pop YL
  ; finally clear the stack
  poptos
  pop temp1
  ; restore status register (and interrupt enable flag)
  out SREG,temp1

  ret

; 
DO_STOREI_atmega:
  ; write data to temp page buffer
  ; use the values in tosl/tosh at the
  ; appropiate place
  rcall pageload

  ; erase page if needed
  ; it is needed if a bit goes from 0 to 1
  com temp4
  com temp5
  and tosl, temp4
  and tosh, temp5
  or tosh, tosl
  breq DO_STOREI_writepage 

    movw ZL, temp2
    ldi temp0,(1<<PGERS)
    rcall dospm

DO_STOREI_writepage:
  ; write page
  movw ZL, temp2
  ldi temp0,(1<<PGWRT)
  rcall dospm

  ; reenable RWW section
  movw ZL, temp2
  ldi temp0,(1<<RWWSRE)
  rcall dospm
  movw ZL, temp2
  adiw ZL, 1
  ret

; load the desired page
.equ pagemask, ~ ( PAGESIZE - 1 )
pageload:
  movw ZL, temp2
  ; get the beginning of page
  andi ZL,lo8(pagemask)
  andi ZH,hi8(pagemask)
  movw YL, ZL
  ; loop counter (in words)
  ldi XL,lo8(PAGESIZE)
  ldi XH,hi8(PAGESIZE)
pageload_loop:
  ; we need the current flash value anyways
  movw ZL, YL
  lsl ZL
  rol ZH
  lpm temp6, Z+
  lpm temp7, Z+
  ; now check: if Z points to the same cell as temp2/3, we want the new data
  movw ZL, YL
  cp ZL, temp2
  cpc ZH, temp3
  breq pageload_newdata
    movw r0, temp6
    rjmp pageload_cont
pageload_newdata:
    movw temp4, temp6
    movw r0, tosl
pageload_cont:
  clr temp0
  rcall dospm
  adiw YL, 1
  sbiw XL, 1
  brne pageload_loop

pageload_done:
  ret


;; dospm
;;
;; EXEC spm instruction
;;   temp0 holds the value for SPMCR

dospm:
dospm_wait_ee:
  sbic EECR, EEPE
  rjmp dospm_wait_ee
dospm_wait_spm:
  in_  temp1, SPMCSR
  sbrc temp1, SPMEN
  rjmp dospm_wait_spm

  ; turn the word addres into a byte address
  writeflashcell

  ; EXEC spm
  ori temp0, (1<<SPMEN)
  out_ SPMCSR,temp0
  spm
  ret

; ( f-addr -- n1 ) 
; Memory
; read 1 cell from flash
forthword_ FETCHI, 0x8002, "@i"
    movw ZL, tosl

ZDFETCHI:
    lsl ZL
    rol ZH

ZFETCHI:
	lpm tosl, Z+
	lpm tosh, Z+
    ret

.include "dict_compiler.inc"

; ( a-addr u c -- ) 
; Memory
; fill u bytes memory beginning at a-addr with character c
forthword_ FILL, 0x8004, "fill"
    rcall RROT      ; ( c a-addr u )
    niptoa_         ; ( c u ) A: a-addr
PFA_FILL1:
    dupzerosense_
    breq PFA_FILL2
    rcall OVER      ; ( c u c )
    rcall ACSTORE   ; ( c u )
    aplus_
    oneminus_       ; ( c u-1 )
    rjmp PFA_FILL1
PFA_FILL2:
    rjmp TWODROP

; ( u -- ) 
; Numeric IO
; unsigned PNO with single cell numbers
forthword_ UDOT, 0x8002, "u."
    rcall ZERO   ; ( n 0 ) want unsigned
    rcall TUCK   ; ( 0 n 0 )
    call DOTR
    jmp SPACE
; : u.        ( us -- )    0 u. ;

; ( -- ) 
; Tools
; stack dump
forthword_ DOTS, 0x8002, ".s"
    rcall SP_FETCH        ; ( limit ) setup limit
    twominus_
    call SP0              ; ( limit counter )
    ; <begin>
PFA_DOTS1:
    twominus_             ; ( limit counter-2 )
    rcall TWOOVER            ; ( limit counter-2 limit counter-2 )
    rcall NOTEQUAL           ; ( limit counter-2 flag )
    ; <while>
    zerosense_
    breq PFA_DOTS2
    dup_                  ; ( limit counter-2 counter-2 )
    rcall FETCH           ; ( limit counter-2 val )
    rcall UDOT            ; ( limit counter-2 )
    ; <repeat>
    rjmp PFA_DOTS1
    
PFA_DOTS2:
    rjmp TWODROP





; ( -- ) System
; R( -- )
; application specific turnkey action
forthword_ APPLTURNKEY, 0x8004, "atky"
    call USART
    call INTON
    jmp VER

; ( a-addr -- n ) 
; Memory
; read 1 cell from RAM address
forthword_ FETCH, 0x8001, "@"
    movw ZL, tosl

ZFETCH:
    ; low byte is read before the high byte
    ld tosl, Z+
    ld tosh, Z+
    ret

; ( n addr -- ) 
; Memory
; write n to RAM memory at addr, low byte first
forthword_ STORE, 0x8001, "!"
    movw ZL, tosl
    poptos

ZSTORE:
    st Z+, tosl
    st Z+, tosh
    poptos
    ret

; ( c a-addr -- ) 
; Memory
; store a single byte to RAM address
forthword_ CSTORE, 0x8002, "c!"
    movw ZL, tosl
    poptos

ZCSTORE:
    st Z+, tosl
    poptos
    ret

; ( a-addr - c1 ) 
; Memory
; fetch a single byte from memory mapped locations
forthword_ CFETCH, 0x8002, "c@"
    movw ZL, tosl

ZCFETCH:
    clr tosh
    ld tosl, Z+
    ret

; ( a-addr -- n ) 
; Memory
; read 1 cell from user RAM address
forthword_ FETCHU, 0x8002, "@u"
    rcall UP_FETCH
    rcall PLUS
    rjmp FETCH

; ( n addr -- ) 
; Memory
; write n to RAM memory at addr, low byte first
forthword_ STOREU, 0x8002, "!u"
    rcall UP_FETCH
    rcall PLUS
    rjmp STORE


; ( -- n2 ) 
; Extended VM
; Read memory pointed to by register A (Extended VM)
forthword_ AFETCH, 0x8002, "a@"
    movw ZL, al
    pushtos
    rjmp ZFETCH

; ( -- n2 ) 
; Extended VM
; Read program memory pointed to by register A (Extended VM)
forthword_ AFETCHI, 0x8003, "a@i"
    movw ZL, al
    pushtos
    rjmp ZFETCHI


; ( n1 -- n2 )
; Extended VM
; Read memory pointed to by register A plus offset (Extended VM)
;forthword_ NAFETCH, 0x8003, "na@"
;    movw ZL, al
;    add ZL, tosl
;    adc ZH, tosh
;    rjmp ZFETCH


; ( -- n ) 
; Extended VM
; Read a byte from ram pointed to by register A (Extended VM)
forthword_ ACFETCH, 0x8004, "ac@"
    movw ZL, al
    pushtos
    rjmp ZCFETCH


; ( n -- )
; Extended VM
; Move TOS to A register (Extended VM)
forthword_ TO_A, 0x8002, ">a"
    copytoa_
    poptos
    ret

; ( n -- )
; Extended VM
; Move TOS to B register (Extended VM)
forthword_ TO_B, 0x8002, ">b"
    copytob_
    poptos
    ret

; ( n -- )
; Extended VM
; copy TOS to A register (Extended VM)
forthword_ _A, 0x8102, ":a"
    copytoa_
    ret

; ( n -- )
; Extended VM
; copy TOS to B register (Extended VM)
forthword_ _B, 0x8102, ":b"
    copytob_
    ret

; ( n1 n2 -- n2 ) A: n1
; Extended VM
; Remove next on stack and write to A register (Extended VM)
forthword_ NIPTOA, 0x8205, "nip>a"
    niptoa_
    ret

; ( n1 n2 -- n2 ) B: n1
; Extended VM
; Remove next on stack and write to B register (Extended VM)
forthword_ NIPTOB, 0x8205, "nip>b"
    niptob_
    ret

; ( n1 -- n2 ) 
; Extended VM
; read the A register (Extended VM)
forthword_ A_, 0x8001, "a"
    pushtos
    movw tosl, al
    ret

; ( n1 -- n2 ) 
; Extended VM
; read the B register (Extended VM)
forthword_ B_, 0x8001, "b"
    pushtos
    movw tosl, bl
    ret


; ( n a-addr -- ) 
; Memory
; add n to content of RAM address a-addr
forthword_ PLUSSTORE, 0x8002, "+!"
    movw ZL, tosl
    poptos
    ld temp2, Z
    ldd temp3, Z+1
    add tosl, temp2
    adc tosh, temp3
    rjmp ZSTORE
    

; ( n -- )
; Extended VM
; Write memory pointed to by register A (Extended VM)
forthword_ ASTORE, 0x8002, "a!"
    movw ZL, al
    rjmp ZSTORE

; ( n -- )
; Extended VM
; Write char memory pointed to by register A (Extended VM)
forthword_ ACSTORE, 0x8003, "ac!"
    movw ZL, al
    rjmp ZCSTORE


; ( n offs -- )
; Extended VM
; Write TOS to memory pointed to by register A plus offset (Extended VM)
forthword_ NASTORE, 0x8003, "na!"
    movw ZL, al
    add ZL, tosl
    adc ZH, tosh
    poptos
    rjmp ZSTORE

; ( -- )
; Extended VM
; update register A with post increment from last memory operation (Extended VM)
forthword_ APLUS, 0x8102, "a+"
    aplus_
    ret
   
; ( -- )
; ISR
; define the start of an Interrupt Service Routine
; must be called
forthword_ ISTART, 0x8005 | DIS_CALL_OPT, "istart"
    ; save important registers
    movw isrZL, ZL
    movw isrXL, XL
    movw isrt0, temp0
    movw isrt2, temp2
    
    ; pop return address
    pop ZH
    pop ZL
    
    ; save status register
    in temp0, SREG
    push temp0
    ijmp
; ( -- )
; ISR
; define the end of an Interupt Service Routine
forthword_ IEND, 0x8004, "iend"
    movw ZL, isrZL
    movw XL, isrXL
    movw temp0, isrt0
    movw temp2, temp2
    
    pop isrt0
    out SREG, isrt0    
    reti
