; dict_minimum.inc
; words needed to make up minimum forth system. These are placed
; in low flash

; ( -- a-addr )
; Numeric IO
; location of the cell containing the number conversion radix

forthword_ BASE, 0, "base"
    call DOUSER
    .word USER_BASE


; ( -- e-addr )
; System Value
; address of the next free address in eeprom
forthword_ EDP, 0, "edp"
    call DOVALUE
    .word EE_EDP
    addr_ FETCHE
    addr_ STOREE

; ( -- addr )
; System Value
; address of the next free data space (RAM) cell
forthword_ HERE, 0, "here"
    call DOVALUE
    .word EE_HERE
    addr_ FETCHE
    addr_ STOREE

; ( n -- )
; Numeric IO
; save base
forthword_ BASESTORE, 0, "base!"
    rcall BASE
    jmp STORE

; ( n -- )
; Numeric IO
; load base
forthword_ BASEFETCH, 0, "base@"
    rcall BASE
    jmp FETCH

; ( -- )
; Numeric IO
; set base for number conversion to 2

forthword_ BIN, 0, "bin"
    call TWO
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for numeric conversion to 10
forthword_ DECIMAL, 0, "decimal"
    call TEN
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for number conversion to 16
forthword_ HEX, 0, "hex"
    doliteral_ 16
    rjmp BASESTORE
    

; ( -- n*y )
; System Value
; Deferred action during startup/reset

forthword_ TURNKEY, 0, "turnkey"
    call DODEFER
    .word EE_TURNKEY
    addr_ FETCHE
    addr_ STOREE


; ( n1 n2 -- n1|n2 )
; Compare
; compare two values leave the smaller one
forthword_ MIN, 0, "min"
    call TWOOVER
    call GREATER
    ; <if>
MIN_TEST:
    zerosense_
    breq MIN_exit
      call SWAP
      
MIN_exit:
    drop_
    ret

; ( n1 n2 -- n1|n2 )
; Compare
; compare two values, leave the bigger one
forthword_ MAX, 0, "max"
    call TWOOVER
    call LESS
    rjmp MIN_TEST


; ( val -- char )
; convert low byte of val to a printable hex character    
forthword_ NHEX, 0, "#h"
    zerohigh_
    andi tosl, 0x0F
    cpi tosl, 10
    brlo NHEX_NEXT
    adiw tosl, 7
    
    ; <then>
NHEX_NEXT:
    adiw tosl, 48  
    ret            

; ( n -- )
; simple 4 bit hex print
forthword_ PNIB, 0, ".h"
    rcall NHEX
    jmp EMIT
    
; ( n -- )
; simple 16 bit hex print    
forthword_ PHEX, 0, ".$"
    doliteral_ '$
    call EMIT
    dup_
    highbyte_
    swapnib_
    rcall PNIB
    dup_
    highbyte_
    rcall PNIB
    dup_
    swapnib_
    rcall PNIB
    dup_
    rcall PNIB
    zerohigh_
    ldi tosl, 32
    jmp EMIT
    

; ( c -- (number|) flag )
; Numeric IO
; tries to convert a character to a number, set flag accordingly
forthword_ DIGITQ, 0, "digit?"
    sbiw tosl, 0x30
    cpi tosl, 10
    brlo PFA_DIGITQ0
    sbiw tosl, 7
    cpi tosl, 10
    brge PFA_DIGITQ0
    drop_
    jmp ZERO

PFA_DIGITQ0:
    dup_
    rcall BASEFETCH
    call UGREATEREQUAL
    zerosense_
    breq PFA_DIGITQ2
    drop_
    jmp ZERO

PFA_DIGITQ2:
    jmp TRUE

; ( u1 u2 -- flag )
; Compare
; compare two unsigned numbers, returns true flag if u1 is less then or equal to u2
forthword_ ULESSEQUAL, 0, "u<="
    call UGREATER
    not_
    ret

; ( u1 u2 -- flag )
; Compare
; compare two unsigned numbers, returns true flag if u1 is greater then or equal to u2
forthword_ UGREATEREQUAL, 0, "u>="
    call ULESS
    not_
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; USER variable used by catch/throw
forthword_ HANDLER, 0, "handle"
    call DOUSER
    .word USER_HANDLER

; ( i*x xt -- j*x 0 | i*x n )
; Exceptions
; setup handler to catch exceptions and then EXEC XT.

forthword_ CATCH, 0, "catch"
    ; sp@ >r
    call SP_FETCH       ; ( xt SP )
    call TO_R           ; ( xt ) (R: ret -- callerret SP )
    ; handler @ >r
    rcall HANDLER       ; ( xt haddr )
    call FETCH          ; ( xt hxt )
    call TO_R           ; ( xt ) (R: callerret SP hxt )
    ; rp@ handler !
    call RP_FETCH       ; ( xt RP ) (R: callerret SP hxt)
    rcall HANDLER       ; ( xt RP haddr )
    call STORE          ; ( xt )
    call EXEC
    ; restore handler
    ; r> handler !
    call R_FROM         ; ( hxt ) (R: callerret SP )
    rcall HANDLER       ; ( hxt haddr )
    call STORE          ; ( )
    r_drop_             ; ( ) (R: callerret)
    jmp ZERO

; ( $addr len -- )
; Exceptions
; throw an exception
; assumes jmp to get here
; will type the string passed in
forthword_ THROW, 0, "throw"
    call ITYPE
    rcall HANDLER      ; ( haddr )
    call FETCH         ; ( RP_handler )
    call RP_STORE      ; ( ) (R: callerret SP hxt)
    call R_FROM        ; ( hxt ) (R: callerret SP )
    rcall HANDLER      ; ( hxt haddr )
    call STORE         ; ( )
    call R_FROM        ; ( SP ) (R: callerret )
    call SP_STORE      ; ( ... )
    jmp ONE


; ( -- )
; send Ctrl-Q (XON) to resume transmission
;forthword_ XON, 0, "XON"
;    doliteral_ 17
;    rjmp EMIT

; send Ctrl-S (XOFF) to pause transmission
;forthword_ XOFF, 0, "XOFF"
;    doliteral_ 19
;    rjmp EMIT



; (c -- ) Numeric IO
; R( -- )
; set the BASE value depending on the character
;forthword_ SETBASE, 0, "setbase"
SETBASE:        ; ( c -- )
    mov temp0, tosl
    drop_
    cpi temp0, '$
    brne PFA_SETBASE0
    rjmp HEX

PFA_SETBASE0:
    cpi temp0, '%
    brne PFA_SETBASE1
    rjmp BIN

PFA_SETBASE1:
    cpi temp0, '&
    brne PFA_SETBASE2
    rjmp DECIMAL

PFA_SETBASE2:        ; ( error)
    dup_
    movw tosl, temp0
    call EMIT
    call DOSLIT
    ascii_ " bad base!"
    rjmp THROW

; ( addr len -- addr' len' )
; Numeric IO
; skip a numeric prefix character
;forthword_ PRAEFIX, 0, "praefix"
PRAEFIX:        ; ( adr1 len1 -- adr2 len2 )
    call OVER
    call CFETCH
    cpi tosl, 0x30 
    brlo PFA_PRAEFIX0
    ; no praefix
    drop_
    ret

PFA_PRAEFIX0:
    rcall SETBASE
    call ONE
    jmp SLASHSTRING

; (addr len -- addr len flag) Numeric IO
; check for - sign
; forthword_ NUMBERSIGN, 0, "numbersign"
NUMBERSIGN:        ; ( addr len -- )
    call OVER      ; ( addr len addr )
    call CFETCH    ; ( addr len char )
    cpi tosl, '-
    breq NUMBERSIGN_HASSIGN
      zerotos_
      ret
NUMBERSIGN_HASSIGN:
      call TO_R
      call ONE
      call SLASHSTRING
      call R_FROM
      ret

; ( u1 c-addr1 len1 -- u2 c-addr2 len2 )
; Numeric IO
; convert a string to a number  c-addr2/u2 is the unconverted string
forthword_ TO_NUMBER, 0, ">num"
    dupzerosense_
    breq PFA_TO_NUMBER1
        call OVER            ; ( u adr len adr)
        call CFETCH          ; ( u adr len char)
        rcall DIGITQ         ; ( u adr len digit flag)
        zerosense_
        brne PFA_TO_NUMBER2
            ; character is not a recognized number
            ret
PFA_TO_NUMBER2:
        call TO_B            ; ( u adr len) B: digit
        call ROT             ; ( adr len u)
        rcall BASEFETCH      ; ( adr len u base)
        call STAR            ; ( adr len u*base)
        call B_              ; ( adr len u' digit)
        call PLUS            ; ( adr len u')
        call RROT            ; ( u' adr len )
        call ONE
        call SLASHSTRING
        rjmp TO_NUMBER

PFA_TO_NUMBER1:
    ret
    
; (addr len -- [n] f)
; Numeric IO
; convert a string at addr to a number
forthword_ NUMBER, 0, "num"
    rcall BASEFETCH
    call TO_R               ; ( addr len ) (R: base)
    rcall NUMBERSIGN
    call TO_R               ; ( addr len ) (R: base flagsign)
    rcall PRAEFIX
    rcall NUMBERSIGN        ; ( addr len flagsign2 )
    call R_FROM             ; ( addr len flagsign2 flagsign ) (R: base )
    call OR                 ; ( addr len flagsign' )
    call TO_R               ; ( addr len ) (R: base flagsign')
    call ZERO               ; ( addr len 0 ) starting value
    call RROT               ; ( 0 addr len )
    rcall TO_NUMBER         ; ( n addr' len' )
    ; check length of the remaining string.
    ; if zero: a single cell number is entered
    zerosense_
	breq PFA_NUMBER1

    ; error in string to number conversion
PFA_NUMBER2:
	nip_                    ; ( addr' )  (R: base flagsign' )
	r_drop_                 ; ( addr' ) (R: base )
	zerotos_                ; ( 0 ) (R: base )
    rjmp PFA_NUMBER5

PFA_NUMBER1:
    drop_                   ; ( n )  (R: base flagsign' )
    ; incorporate sign into number
    call R_FROM             ; ( n flagsign' ) (R: base )
    zerosense_
    breq PFA_NUMBER4
    call NEGATE
PFA_NUMBER4:
    call TRUE               ; ( n true ) (R: base )
PFA_NUMBER5:
    call R_FROM             ; ( n true base ) (R: )
    rjmp BASESTORE          ; ( n true )


; allocate space for User Area
.pushsection .noinit
; MCUSR from last cold execution
ram_mcusro: .space 2
ram_user1: .space SYSUSERSIZE
.popsection

; ( -- ram_mcusr )
; get mcusro addr. this is the mcusr read and stored when cold executes.
forthword_ RAM_MCUSRO, 0, "mcusro"
    doliteral_ ram_mcusro
    ret

; ( -- )
; System
; start up asforth.
forthword_ COLD, 0, "cold"
    in_ r10, MCUSR
    clr r11
    clr zerol
    out_ MCUSR, zerol
    ; init first user data area
    ldi ZL, lo8(ram_user1)
    ldi ZH, hi8(ram_user1)
    movw upl, ZL
    ; init return stack pointer
	; using the MCU SP registers
    ldi temp0, lo8(rstackstart)
    out_ SPL,temp0
    ldi temp1, hi8(rstackstart)
    out_ SPH,temp1

    ; init parameter stack pointer
    ldi YL, lo8(stackstart)
    ldi YH, hi8(stackstart)
    pushtos
    movw tosl, r10
    rcall RAM_MCUSRO
    call STORE
    rjmp WARM


; ( nx* -- ) (R: ny* -- )
; System
; initialize asforth further. EXECs turnkey operation and go to quit
forthword_ WARM, 0, "warm"
    rcall INITUSER
    doliteral_ pm(NOOP)
    doliteral_ pm(PAUSE)
    call DEFERSTORE
    ; init DP in ram from eeprom
    call DPFETCHE
    call ONLY
    rcall USART
    rcall INTON
    call DOSLIT
    ascii_ "asforth 1.2"
    call ITYPE
    call CR
    
    rcall TURNKEY
    jmp QUIT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr)
; Stack
; start address of the data stack
forthword_ SP0, 0, "sp0"
    doliteral_ stackstart
    ret
    
; ( -- addr)
; Stack
; start address of return stack
forthword_ RP0, 0, "rp0"
    doliteral_ rstackstart
    ret



; ( r-addr r-len f-addr f-len --  f) 
; String
; compares two strings in RAM

;forthword_ COMPARE, 0, "compare"
;    movw temp0, tosl
;    poptos
;    movw XL, tosl
;    poptos
;    movw temp2, tosl
;    poptos
;    movw ZL, tosl
;PFA_COMPARE_LOOP:
;    ld temp4, X+
;    ld temp5, Z+
;    cp temp4, temp5
;    brne PFA_COMPARE_NOTEQUAL
;    dec temp0
;    breq PFA_COMPARE_ENDREACHED2
;    dec temp2
;    brne PFA_COMPARE_LOOP
;    rjmp PFA_COMPARE_ENDREACHED
;PFA_COMPARE_ENDREACHED2:
;    dec temp2
;PFA_COMPARE_ENDREACHED:
;    or temp0, temp2
;    brne PFA_COMPARE_CHECKLASTCHAR
;    clr tosl
;    rjmp PFA_COMPARE_DONE
;PFA_COMPARE_CHECKLASTCHAR:
;PFA_COMPARE_NOTEQUAL:
;    ser tosl

;PFA_COMPARE_DONE:
;    mov tosh, tosl
;    ret




; (addr-from addr-to n -- )
; Memory
; copy data in RAM, from lower to higher addresses

;forthword_ CMOVE, 0, "cmove"
;    ld ZL, Y+
;    ld ZH, Y+ ; addr-to
;    ld XL, Y+
;    ld XH, Y+ ; addr-from
;    mov temp0, tosh
;    or temp0, tosl
;    brbs 1, PFA_CMOVE1
;PFA_CMOVE2:
;    ld temp1, X+
;    st Z+, temp1
;    oneminus_
;    brbc 1, PFA_CMOVE2
;PFA_CMOVE1:
;    poptos
;    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;

; (addr-from addr-to n -- ) 
; Memory
; copy data in RAM from higher to lower addresses.

;forthword_ CMOVE_G, 0, "c>"
;    ld ZL, Y+
;    ld ZH, Y+ ; addr-to
;    ld XL, Y+
;    ld XH, Y+ ; addr-from
;    mov temp0, tosh
;    or temp0, tosl
;    brbs 1, PFA_CMOVE_G1
;    add ZL, tosl
;    adc ZH, tosh
;    add XL, tosl
;    adc XH, tosh
;PFA_CMOVE_G2:
;    ld temp1, -X
;    st -Z, temp1
;    oneminus_
;    brbc 1, PFA_CMOVE_G2
;PFA_CMOVE_G1:
;    poptos
;    ret


; ( ee-addr -- itemn .. item0 n) 
; Tools
; Get an array from EEPROM and put on the stack
;forthword_ FETCH_EE_ARRAY, 0, "@e[]"
;    dup_               ; ( ee-addr ee-addr )
;    rcall FETCHE       ; ( ee-addr len )
;    rcall _A           ; ( ee-addr len ) A: len
;    twostar_           ; ( ee-addr blen )
    ; go from bigger to smaller addresses
;PFA_FETCH_EE_ARRAY1:
;    copytob_           ; ( ee-addr2 blen )
;    dupzerosense_      ; ( ee-addr2 blen)
;    breq PFA_FETCH_EE_ARRAY2
;    rcall OVER         ; ( ee-addr2 blen ee-addr2 )
;    rcall PLUS          ; ( ee-addr2 ee-addr3 )
;    rcall FETCHE       ; ( ee-addr2 item_i )
;    rcall SWAP         ; ( item_i ee-addr2 )
;    rcall B_           ; ( item_i ee-addr2-2 blen)
;    twominus_          ; ( item_i ee-addr2-2 blen-2)
;    rjmp PFA_FETCH_EE_ARRAY1
;PFA_FETCH_EE_ARRAY2:
;    rcall DDROP       ; ( ... item_i )
;    rjmp A_            ; ( ... item_i len )

; ( recn .. rec0 n ee-addr -- ) 
; Tools
; Write a list to EEPROM
;forthword_ STORE_EE_ARRAY, 0, "!e[]"
;    rcall TWOOVER   ; ( recn .. rec0 n ee-addr n ee-addr )
;    rcall STOREE ; ( recn .. rec0 n e-addr )
;    rcall SWAP   ; ( recn .. rec0 e-addr n )
;PFA_STORE_EE_ARRAY1:
;    copytob_       ; ( recn .. rec0 e-addr n ) B: n
;    zerosense_
;    breq PFA_STORE_EE_ARRAY2
;    twoplus_       ; ( i_x e-addr+2 )
;    rcall TUCK     ; ( e-addr+2 i_x e-addr+2 )
;    rcall STOREE   ; ( e-addr+2 )
;    rcall B_       ; ( e-addr+2 n )
;    oneminus_      ; ( e-addr+2 n-1 )
;    rjmp PFA_STORE_EE_ARRAY1
;PFA_STORE_EE_ARRAY2:
;    drop_
;    ret

; ( -- n )
; Stack
; number of single-cell values contained in the data stack before n was placed on the stack.
forthword_ DEPTH, 0, "depth"
    rcall SP0
    call SP_FETCH
    call MINUS
    twoslash_
    oneminus_
    ret

; ( --  )
; Tools
; check stack underflow, throw exception -4
forthword_ QSTACK, 0, "?sp"
    rcall DEPTH
    call LESSZERO
    zerosense_
    brne PFA_QSTACKERROR
    ret
PFA_QSTACKERROR:

      call DOSLIT
      ascii_ " Stack Underflow!"
      rjmp THROW

; ( -- v)
; System Value
; address of the default user area content in eeprom
forthword_ EEUSER, 0, "ee-u"
  doliteral_ EE_INITUSER
  ret

; ( e-addr r-addr len -- )
; Memory
; copy len cells from eeprom to ram
forthword_ EE2RAM, 0, "ee>r"
    niptoa_              ; ( e-addr len ) A: r-addr
    niptob_              ; ( len ) B: e-addr
PFA_EE2RAM_1:
    ; <begin>
    ; <while>
    dupzerosense_
    breq PFA_EE2RAM_2
    call B_              ; (len e-addr)
    dup_                 ; ( len e-addr e-addr )
    call FETCHE          ; ( len e-addr n )
    call ASTORE          ; ( len e-addr )
    aplus_
    twoplus_             ; ( len e-addr+2 )
    call TO_B            ; ( len ) B: e-addr+2
    oneminus_            ; ( len-1 )
    ; <repeat>
    rjmp PFA_EE2RAM_1

PFA_EE2RAM_2:
    jmp DROP

; ( -- )
; Tools
; setup the default user area from eeprom
forthword_ INITUSER, 0, "i-u"
    rcall EEUSER
    call UP_FETCH
    doliteral_ SYSUSERSIZE
    twoslash_
    rjmp EE2RAM
    

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ STAR, 0, "*"
    rcall MSTAR
    drop_
    ret

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ MSTAR, 0, "m*"
    movw temp0, tosl
    poptos
    movw temp2, tosl
    ; high cell ah*bh
    muls temp3, temp1
    movw temp4, r0
    ; low cell  al*bl
    mul  temp2, temp0
    movw tosl, r0
    ; signed ah*bl
    mulsu temp3, temp0
    sbc   temp5, zerol
    add   tosh,  r0
    adc   temp4, r1
    adc   temp5, zerol

    ; signed al*bh
    mulsu temp1, temp2
    sbc   temp5, zerol
    add   tosh,  r0
    adc   temp4, r1
    adc   temp5, zerol

    pushtos
    movw tosl, temp4
    ret

; ( ud u2 -- rem quot)
; Arithmetics
; unsigned division ud / u2 with remainder
forthword_ UMSLASHMOD, 0, "um/mod"
    movw temp4, tosl

    ld temp2, Y+
    ld temp3, Y+

    ld temp0, Y+
    ld temp1, Y+

;; unsigned 32/16 -> 16r16 divide

PFA_UMSLASHMODmod:

  ; set loop counter
    ldi XL, 0x10

PFA_UMSLASHMODmod_loop:
    ; shift left, saving high bit
    clr XH
    lsl temp0
    rol temp1
    rol temp2
    rol temp3
    rol XH

  ; try subtracting divisor
    cp temp2, temp4
    cpc temp3, temp5
    cpc XH,zerol

    brcs PFA_UMSLASHMODmod_loop_control

PFA_UMSLASHMODmod_subtract:
    ; dividend is large enough
    ; do the subtraction for real
    ; and set lowest bit
    inc temp0
    sub temp2, temp4
    sbc temp3, temp5

PFA_UMSLASHMODmod_loop_control:
    dec  XL
    brne PFA_UMSLASHMODmod_loop

PFA_UMSLASHMODmod_done:
    ; put remainder on stack
    st -Y,temp3
    st -Y,temp2

    ; put quotient on stack
    movw tosl, temp0
    ret

   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( n1 -- n2 )
; Logic
; 2-complement
forthword_ NEGATE, INLINE_OPT, "neg"
    not_
    oneplus_
    ret


; code adapted from atmel avr200.asm
DIV16_16:
	clr	temp4        	;clear remainder Low byte
    sub	temp5, temp5    ;clear remainder High byte and carry
    ldi	XL, 17	    ;init loop counter

DIV16_16_1:
	rol	temp0		    ;shift left dividend
    rol	temp1
    dec	XL	    	;decrement counter
    brne DIV16_16_2		;if done
    ret

DIV16_16_2:
	rol	temp4	;shift dividend into remainder
    rol	temp5
    sub	temp4, temp2	;remainder = remainder - divisor
    sbc	temp5, temp3	;
    brcc	DIV16_16_3		;if result negative
    add	temp4, temp2	;    restore remainder
    adc	temp5, temp3
    clc			;    clear carry to be shifted into result
    rjmp	DIV16_16_1		;else

DIV16_16_3:
	sec			;    set carry to be shifted into result
    rjmp	DIV16_16_1

; ( n1 n2 -- rem quot)
; Arithmetics
; unsigned division n1/n2 with remainder and quotient
forthword_ USLASHMOD, 0, "u/mod"
    movw temp2, tosl

    ld temp0, Y+
    ld temp1, Y+

    rcall DIV16_16
    rjmp SLASHMOD_end


; ( n1 n2 -- rem quot)
; Arithmetics
; signed division n1/n2 with remainder and quotient
forthword_ SLASHMOD, 0, "/mod"
    movw temp2, tosl

    ld temp0, Y+
    ld temp1, Y+

    mov	XL, temp1	;move dividend High to sign register
    eor	XL, temp3	;xor divisor High with sign register
    sbrs	temp1, 7	;if MSB in dividend set
    rjmp	SLASHMOD_1
    com	temp1		;    change sign of dividend
    com	temp0
    subi	temp0, lo8(-1)
    sbci	temp1, hi8(-1)

SLASHMOD_1:
    sbrs	temp3, 7	;if MSB in divisor set
    rjmp	SLASHMOD_2
    com	temp3		;    change sign of divisor
    com	temp2
    subi	temp2, lo8(-1)
    sbci	temp3, hi8(-1)

SLASHMOD_2:
    rcall DIV16_16

    sbrs	XL, 7		;    if MSB in sign register set
    rjmp	SLASHMOD_end
    com	temp1            	;        change sign of result
    com	temp0
    subi	temp0, lo8(-1)
    sbci	temp1, hi8(-1)

SLASHMOD_end:
    ; put remainder on stack
    st -Y, temp5
    st -Y, temp4
    ; put quotient on stack
    movw tosl, temp0
    ret


; ( n1 -- u1 )
; Arithmetics
; get the absolute value

forthword_ ABS, 0, "abs"
    dup_
    call LESSZERO
    ; <if>
    zerosense_
    breq ABS_exit
      jmp NEGATE
    ; <then>  
ABS_exit:
    ret

; ( -- icell )
; push instruction cell size 
forthword_ ICELL, INLINE_OPT, "icell"
    call ONE
    ret
  
; ( n -- n-icell )
; add instruction cell size to n
forthword_ ICELLM, INLINE_OPT, "icell-"
    oneminus_
    ret
    
; ( n -- n+icell )
; add instruction cell size to n
forthword_ ICELLP, INLINE_OPT, "icell+"
    oneplus_
    ret
  
; ( -- dcell )
; push data cell size 
forthword_ DCELL, INLINE_OPT, "dcell"
    call TWO
    ret
  
; ( n -- n-dcell )
; add data cell size to n
forthword_ DCELLM, INLINE_OPT, "dcell-"
    twominus_
    ret
    
; ( n -- n+dcell )
; add data cell size to n
forthword_ DCELLP, INLINE_OPT, "dcell+"
    twoplus_
    ret
  
; ( n -- n*dcell )
; multipley data cell size by n
forthword_ DCELLS, INLINE_OPT, "dcell*"
    twostar_
    ret
    
; ( i*x x1 -- | i*x ) (R: j*y -- | j*y ) (C: "ccc<quote>" -- )
; Exceptions
; check flag. If true display the parsed text and throw exception -2 

;forthword_ ABORTSTRING, DIS_CALL_OPT, "abort\""
;    ; postpone if
;    call DOCOMPILE
;    addr_ 0 ; ******* FIXME
;    call DP
;    call DOCOMPILE
;    .word -1
    
;    ;rcall DOTSTRING
    
;    call DOCOMPILE
;    addr_ -1 ; DOLITERAL ******* FIXME
    
;    call DOCOMPILE
;    .word -2
;    call DOCOMPILE
;    addr_ THROW
;    ; then
;    call DP
;    rcall SWAP
;    rjmp STOREI
