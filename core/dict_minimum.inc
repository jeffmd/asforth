;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ( -- addr )
; System Variable
; system state
forthword_ STATE, 0x8005, "state"
    doliteral_ ram_state
    ret

.pushsection .data
ram_state: .space 2
.popsection

; ( -- a-addr )
; Numeric IO
; location of the cell containing the number conversion radix

forthword_ BASE, 0x8004, "base"
    call DOUSER
    addr_ USER_BASE
    ret

; ( -- a-addr )
; System Variable
; pointer to current read position in input buffer
forthword_ G_IN, 0x8003, ">in"
    call DOUSER
    .word USER_G_IN

; ( -- a-addr )
; System Variable
; pointer to current cursor position in input buffer
; its where the next key input will go
;forthword_ CURSOR, 0x8006, "cursor"
;    call DOUSER
;    .word USER_CURSOR


; ( -- addr )
; System Variable
; variable holding the number of characters in TIB
forthword_ SHARPTIB, 0x8004, "#tib"
    doliteral_ ram_sharptib
    ret

.pushsection .data
ram_sharptib: .space 2
.popsection

; ( -- a-addr )
; System Variable
; Address of the temporary scratch buffer.

forthword_ PAD, 0x8003, "pad"
    rcall HERE
    doliteral_ 100
    jmp PLUS

; ( c -- )
; Character IO
; fetch the emit vector and EXEC it. should emit a character from TOS
forthword_ EMIT, 0x8004, "emit"
    call DODEFER
    .word USER_EMIT
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( -- f )
; Character IO
; fetch emit? vector and EXEC it. should return the ready-to-send condition
forthword_ EMITQ, 0x8005, "emit?"
    call DODEFER
    .word USER_EMITQ
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( -- c )
; Character IO
; fetch key vector and EXEC it, should leave a single character on TOS
forthword_ KEY, 0x8003, "key"
    call DODEFER
    .word USER_KEY
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( -- f)
; Character IO
; fetch key? vector and EXEC it. should turn on key sender, if it is disabled/stopped
forthword_ KEYQ, 0x8004, "key?"
    call DODEFER
    .word USER_KEYQ
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( -- f-addr )
; System Value
; address of the next free dictionary cell
forthword_ DP, 0x8002, "dp"
    call DOVALUE
    .word EE_DP
    addr_ FETCHE
    addr_ STOREE

; ( -- e-addr )
; System Value
; address of the next free address in eeprom
forthword_ EDP, 0x8003, "edp"
    call DOVALUE
    .word EE_EDP
    addr_ FETCHE
    addr_ STOREE

; ( -- addr )
; System Value
; address of the next free data space (RAM) cell
forthword_ HERE, 0x8004, "here"
    call DOVALUE
    .word EE_HERE
    addr_ FETCHE
    addr_ STOREE

; ( n -- )
; System
; allocate or release memory in RAM

forthword_ ALLOT, 0x8005, "allot"
    rcall HERE
    call PLUS
    rcall DOTO
    addr_ HERE
    ret

; ( --  ) (C: "<spaces>name" -- voc-link )
; Compiler
; parse the input and create an vocabulary entry without XT and data field (PF)
forthword_ DOCREATE, 0x8008, "(create)"
    rcall PARSENAME
    call GET_CURRENT
    ; fall thru into HEADER
    rjmp HEADER

; ( addr len wid -- voc-link )
; Compiler
; creates the vocabulary header without XT and data field (PF) in the wordlist wid
forthword_ HEADER, 0x8006, "header"
    rcall DP            ; ( addr len wid DP )
    call TO_R           ; ( addr len wid ) (R: DP )
    call TO_R           ; ( addr len ) (R: DP wid )
    ; <if>
    dupzerosense_
    breq HEADER_error
      dup_                ; ( addr len len )
      sbr tosh, 0x80      ; ( addr len len' )
      rcall DOSCOMMA      ; ( )
      ; make voc link
      call R_FROM         ; ( wid ) (R: DP )
      call FETCHE         ; ( VE_HEAD )
      rcall COMMA         ; ( )
      call R_FROM         ; ( DP ) (R: )
      ret

    ; <then>
HEADER_error:
    ; -16: attempted to use zero length string as a name
    doliteral_ -16
    rjmp THROW

; ( "ccc<eol>" -- )
; Compiler
; everything up to the end of the current line is a comment

forthword_ BACKSLASH, 0x0001, "\\"
    rcall SOURCE
    nip_
    rcall G_IN
    jmp STORE

; ( "ccc<paren>" -- )
; Compiler
; skip everything up to the closing bracket on the same line
forthword_ LPARENTHESIS, 0x0001, "("
    doliteral_ 0x29
    rcall PARSE
    jmp TWODROP

; ( -- )
; Dictionary
; read the following cell from the dictionary and append it to the current dictionary position.

forthword_ COMPILE, 0x8007, "compile"
    call R_FETCHPLUS    ; ( raddr ) (R: raddr+1)
    call FETCHI         ; ( xt1 )
    ; don't allow inlining
    call ZERO           ; ( xt1 0 )
    jmp COMPILEXT

; ( distance -- distance flag )
; Tools
; Determines if a relative distance is within a relative jmp/call range
; flag is true if relative distance is between -2048 and 2047
forthword_ QRDIST, 0x8006, "?rdist"
    cpi tosh, 0x08
    brsh QDIST_HTEST
QDIST_INRANGE:
    jmp TRUE
QDIST_HTEST:
    cpi tosh, 0xf8
    brsh QDIST_INRANGE
    jmp ZERO


; ( n -- )
; Dictionary
; compile 16 bit into flash at (dictionary pointer) DP
forthword_ COMMA, 0x8001, ","
    rcall DP
    call STOREI
    rjmp DPPLUSONE

; ( -- )
; Dictionary
; increment 16 bit (dictionary pointer) DP by one
forthword_ DPPLUSONE, 0x8004, "dp+1"
    call ONE
    rjmp DPPLUS

; ( offset -- )
; Dictionary
; add offset to 16 bit (dictionary pointer) DP
forthword_ DPPLUS, 0x8003, "dp+"
    rcall DP
    call PLUS
    rcall DOTO
    addr_ DP
    ret

; ( -- xt ) (C: "<space>name" -- )
; Compiler
; what ' does in the interpreter mode, do in colon definitions

forthword_ BRACKETTICK, 0x0003, "[']"
    rcall COMPILE
    addr_ -1; DOLITERAL ********* FIXME
    rcall TICK
    rjmp COMMA

; (  -- )
; doesn't do anything - no operation
forthword_ NOOP, 0x8004, "noop"
    ret

; ( -- )
; Numeric IO
; set base for number conversion to 2

forthword_ BIN, 0x8003, "bin"
    call TWO
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for numeric conversion to 10
forthword_ DECIMAL, 0x8007, "decimal"
    call TEN
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for number conversion to 16
forthword_ HEX, 0x8003, "hex"
    doliteral_ 16
    rjmp BASESTORE
    
; ( n -- )
; Numeric IO
; save base
forthword_ BASESTORE, 0x8005, "base!"
    rcall BASE
    jmp STORE

; ( -- 32 )
; Character IO
; put ascii code of the blank/space character to the stack

forthword_ BL, 0x8002, "bl"
    doliteral_ 32
    ret

; ( -- n*y )
; System Value
; Deferred action during startup/reset

forthword_ TURNKEY, 0x8007, "turnkey"
    call DODEFER
    .word EE_TURNKEY
    addr_ FETCHE
    addr_ STOREE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( n1 -- n2 )
; Logic
; 2-complement
forthword_ NEGATE, 0x8006, "negate"
    invert_
    oneplus_
    ret

; ( n1 n2 -- n3)
; Arithmetics
; divide n1 by n2. giving the quotient
forthword_ SLASH, 0x8001, "/"
    rcall SLASHMOD
    nip_
    ret

; code adapted from atmel avr200.asm
DIV16_16:
	clr	temp4        	;clear remainder Low byte
    sub	temp5, temp5    ;clear remainder High byte and carry
    ldi	temp7, 17	    ;init loop counter

DIV16_16_1:
	rol	temp0		    ;shift left dividend
    rol	temp1
    dec	temp7	    	;decrement counter
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
forthword_ USLASHMOD, 0x8005, "u/mod"
    movw temp2, tosl

    ld temp0, Y+
    ld temp1, Y+

    rcall DIV16_16
    rjmp SLASHMOD_end


; ( n1 n2 -- rem quot)
; Arithmetics
; signed division n1/n2 with remainder and quotient
forthword_ SLASHMOD, 0x8004, "/mod"
    movw temp2, tosl

    ld temp0, Y+
    ld temp1, Y+

    mov	temp6, temp1	;move dividend High to sign register
    eor	temp6, temp3	;xor divisor High with sign register
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

    sbrs	temp6, 7		;    if MSB in sign register set
    rjmp	SLASHMOD_end
    com	temp1            	;        change sign of result
    com	temp0
    subi	temp0, lo8(-1)
    sbci	temp1, hi8(-1)

SLASHMOD_end:
    ; put remainder on stack
    st -Y,temp5
    st -Y,temp4
    ; put quotient on stack
    movw tosl, temp0
    ret

; ( n1 n2 -- n3)
; Arithmetics
; divide n1 by n2 giving the remainder n3
forthword_ MOD, 0x8003, "mod"
    rcall SLASHMOD
    drop_
    ret

; ( n1 -- u1 )
; Arithmetics
; get the absolute value

forthword_ ABS, 0x8003 "abs"
    dup_
    call LESSZERO
    ; <if>
    zerosense_
    breq ABS_exit
      rjmp NEGATE
    ; <then>  
ABS_exit:
    ret

; ( n1 n2 -- n1|n2 )
; Compare
; compare two values leave the smaller one
forthword_ MIN, 0x8003, "min"
    call TWOOVER
    call GREATER
    ; <if>
    zerosense_
    breq MIN_exit
      call SWAP
      
MIN_exit:
    drop_
    ret

; ( n1 n2 -- n1|n2 )
; Compare
; compare two values, leave the bigger one
forthword_ MAX, 0x8003, "max"
    call TWOOVER
    call LESS
    ; <if>
    zerosense_
    breq MAX_exit
      call SWAP
      
MAX_exit:
    drop_
    ret

; ( n min max -- f)
; Compare
; check if n is within min..max
forthword_ WITHIN, 0x8006, "within"
    call OVER
    call MINUS
    call TO_A
    call MINUS
    call A_
    jmp ULESS

; : within >r over > swap r> > or 0= ;
; alternativly
; : within over - >r - r> u< ;

;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr )
; Numeric IO
; pointer to current write position in the Pictured Numeric Output buffer
forthword_ HLD, 0x8003, "hld"
    doliteral_ ram_hld
    ret

.pushsection .data
ram_hld: .space 2
.popsection

; ( c -- )
; Numeric IO
; prepend character to pictured numeric output buffer

forthword_ HOLD, 0x8004, "hold"
    rcall HLD    ; ( c ram_hld )
    call TO_A    ; ( c ) A: ram_hld)
    call AFETCH  ; ( c hpos )
    oneminus_    ; ( c hpos-1 )
    dup_         ; ( c hpos-1 hpos-1 )
    call ASTORE  ; ( c hpos-1 )
    jmp CSTORE   ; (  )

; ( -- )
; Numeric IO
; initialize the pictured numeric output conversion process
forthword_ L_SHARP, 0x8002, "<#"
    rcall PAD
    rcall HLD
    jmp STORE

; ( u1 -- u2 )
; Numeric IO
; pictured numeric output: convert one digit
forthword_ SHARP, 0x8001, "#"
    rcall BASE
    call FETCH          ; ( u1 base )
    rcall USLASHMOD       ; ( rem u2 )
    call SWAP           ; ( u2 rem )
    doliteral_ 9                 ; ( u2 rem 9 )
    call OVER           ; ( u2 rem 9 rem )
    call LESS           ; ( u2 rem flag )
    ; <if>
    zerosense_
    breq PFA_SHARP1
    doliteral_ 7        ; ( u2 rem 7 )
    call PLUS           ; ( u2 char )
    
    ; <then>
PFA_SHARP1:
    doliteral_ 0x30              ; ( u2 char 0x30 )
    call PLUS           ; ( u2 char )
    rjmp HOLD           ; ( u2 )

; ( u -- 0 )
; Numeric IO
; pictured numeric output: convert all digits until 0 (zero) is reached
forthword_ SHARP_S, 0x8002, "#s"
    ; <begin>
    rcall SHARP          ; ( u )
    ; <until>
    dupzerosense_
    brne SHARP_S
    ret

; ( u1 -- addr count )
; Numeric IO
; Pictured Numeric Output: convert PNO buffer into an string
forthword_ SHARP_G, 0x8002, "#>"
    drop_
    rcall HLD
    call FETCH
    dup_
    rcall PAD
    call SWAP
    jmp MINUS

; ( n -- )
; Numeric IO
; place a - in HLD if n is negative
forthword_ SIGN, 0x8004, "sign"
    call LESSZERO
    ; <if>
    zerosense_
    breq SIGN_POS
      doliteral_ 0x2d
      rjmp HOLD
      
    ; <then>
SIGN_POS:
    ret

; ( n -- )
; Numeric IO
; singed PNO with single cell numbers
forthword_ DOT, 0x8001, "."
    dup_             ; ( n n )
    rcall ABS        ; ( n n' )
    call ZERO        ; ( n n' 0 ) not right aligned
    rcall DOTR
    rjmp SPACE
; : .         ( s -- )    .r ;

; ( wantsign n w -- )
; Numeric IO
; singed PNO with cell numbers, right aligned in width w
forthword_ DOTR, 0x8002, ".r"
    call TO_R        ; ( wantsign n ) (R: w)
    rcall L_SHARP
    rcall SHARP_S     ; ( wantsign 0 )
    call SWAP        ; ( 0 wantsign )
    rcall SIGN        ; ( 0 )
    rcall SHARP_G     ; ( addr len )
    call R_FROM      ; ( addr len w )  ( R: )
    call OVER        ; ( addr len w len )
    call MINUS       ; ( addr len spaces )
    rcall SPACES      ; ( addr len )
    rjmp TYPE        ; ( )
; : .r       ( n w -- )
;             >r swap over abs <# #s rot sign #> r> over - spaces type ;

; ( c -- (number|) flag )
; Numeric IO
; tries to convert a character to a number, set flag accordingly
forthword_ DIGITQ, 0x8006, "digit?"
    ;rcall TOUPPER
    doliteral_ 0x30
    call MINUS
    dup_
    doliteral_ 0x09
    call UGREATER
    zerosense_
    breq PFA_DIGITQ0
    doliteral_ 0x07
    call MINUS
    dup_
    doliteral_ 0x09
    call ULESSEQUAL
    zerosense_
    breq PFA_DIGITQ0
    drop_
    jmp ZERO

PFA_DIGITQ0:
    dup_
    rcall BASE
    call FETCH
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
forthword_ ULESSEQUAL, 0x8003, "u<="
    call UGREATER
    invert_
    ret

; ( u1 u2 -- flag )
; Compare
; compare two unsigned numbers, returns true flag if u1 is greater then or equal to u2
forthword_ UGREATEREQUAL, 0x8003, "u>="
    call ULESS
    invert_
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr len )
; String
; runtime portion of sliteral
; string literal is located starting at return cell
; return cell is string length
; string then occupies the following cells: c2c1 c4c3 ...

;forthword_ DOSLITERAL, 0x800a, "(sliteral)"
DOSLITERAL:
  ; get return address to get string length, calculate string address start,
  ; and calculate return address
  call R_FROM   ; ( -- raddr )  (R: raddr -- )
  dup_      ; ( -- raddr raddr )
  dup_      ; ( -- raddr raddr raddr )
  ; get the string length
  call FETCHI   ; ( -- raddr raddr slength )
  call SWAP     ; ( -- raddr slength raddr )
  ; calculate string address
  oneplus_    ; ( -- raddr slength straddr )
  ; calculate the number of words to skip over for return address
  call FLIP     ; ( -- straddr slength raddr )
  call OVER     ; ( -- straddr slength raddr slength)
  call WALIGN   ; ( -- straddr slength raddr k/2 )
  call PLUS     ; ( -- straddr slength raddr+k/2 )
  ; also skip string length
  oneplus_    ; ( -- straddr slength raddr+k/2+1 )
  call TO_R      ; ( -- )
  ret

; ( addr len -- )
; Compiler
; compiles a string from RAM to Flash
forthword_ SCOMMA, 0x8002, "s,"
    dup_
    ; fall thru into DOSCOMMA
    rjmp DOSCOMMA


; ( len -- len/2 rem)
; Arithmetics
; convert byte length to word length plus remainder
forthword_ BTOW, 0x8004, "btow"
    dup_        ; ( -- len len)
    twoslash_   ; ( -- len len/2
    call TUCK   ; ( -- len/2 len len/2
    twostar_    ; ( -- len/2 len len'
    call MINUS  ; ( -- len/2 rem
    ret


; ( addr len len' -- )
; Compiler
; compiles a string from RAM to Flash
;forthword_ DOSCOMMA, 0x8004, "(s,)"
DOSCOMMA:
    rcall COMMA        ; ( addr len )
    rcall BTOW         ; ( addr len/2 rem )
    call TO_R          ; ( addr len/2 ) (R: rem )
    niptoa_            ; ( len/2 ) A: addr

    ; begin
PFA_SCOMMA1:
    ; ?while
    dupzerosense_      ; ( len )
    breq PFA_SCOMMA2
    call AFETCH        ; ( len c1c2 )
    aplus_
    rcall COMMA        ; ( len )
    oneminus_          ; ( len-1 )
    ; repeat
    rjmp PFA_SCOMMA1

PFA_SCOMMA2:
    drop_              ; (  )
    call R_FROM        ; ( rem ) (R: )
    ; if
    zerosense_         ; ( )
    breq PFA_SCOMMA3
      call ACFETCH     ; ( c1 )
      rcall COMMA      ; ( )

    ; then
PFA_SCOMMA3:
    ret

; ( addr n --  )
; Tools
; reads string from flash and prints it
forthword_ ITYPE, 0x8005, "itype"
    rcall BTOW       ; ( --addr len/2 rem )
    call TO_B        ; ( addr len/2 ) B: rem
    call SWAP        ; ( len/2 addr )
    twostar_         ; ( len/2 addr*2 )
    call TO_A        ; ( len/2 ) A: addr*2
    ; begin
PFA_ITYPE1:
    ; ?while
    dupzerosense_
    breq PFA_ITYPE2
    call AFETCHI          ; ( len c1c2 )
    aplus_                ; a+
    dup_                  ; ( len c1c2 c1c2 )
    rcall EMIT            ; ( len c1c2 )
    call BYTESWAP         ; ( len c2c1 )
    rcall EMIT            ; ( len )
    oneminus_             ; ( len-1 )
    ; repeat
    rjmp PFA_ITYPE1

PFA_ITYPE2:
    call DROP             ; ( )
    call B_               ; ( rem )
    call GREATERZERO      ; ( flag )
    zerosense_
    breq PFA_ITYPE3
      call AFETCHI
      rcall EMIT
PFA_ITYPE3:
    ret

; ( addr -- addr+1 n )
; Tools
; get string address and length information out of a counted string in flash
forthword_ ICOUNT, 0x8002, "$l"
    dup_
    oneplus_
    call SWAP
    jmp FETCHI

; ( -- )
; Character IO
; cause subsequent output appear at the beginning of the next line
forthword_ CR, 0x8002, "cr"
    doliteral_ 13
    rcall EMIT
    call TEN
    rjmp EMIT

; ( -- )
; Character IO
; emits a space (bl)
forthword_ SPACE, 0x8005, "space"
    rcall BL
    rjmp EMIT

; ( n -- )
; Character IO
; emits n space(s) (bl)
; only accepts positive values
forthword_ SPACES, 0x8006, "spaces"
    ; make sure a positive number
    dup_
    call GREATERZERO
    call AND
PFA_SPACES1:
    dupzerosense_
    breq PFA_SPACES2
    rcall SPACE
    oneminus_
    rjmp PFA_SPACES1
PFA_SPACES2:
    jmp DROP

; ( addr n -- )
; Character IO
; print a RAM based string
forthword_ TYPE, 0x8004, "type"
    niptoa_         ; ( n ) A: addr
PFA_TYPE1:
    dupzerosense_
    breq PFA_TYPE2
    call ACFETCH     ; ( n c )
    aplus_           ; a+
    rcall EMIT       ; ( n )
    oneminus_        ; ( n-1 )
    rjmp PFA_TYPE1
PFA_TYPE2:
    jmp DROP

; ( "<spaces>name" -- XT )
; Dictionary
; search dictionary for name, returns XT or throw an exception -13
forthword_ TICK, 0x8001, "'"
    rcall PARSENAME
    rcall FINDNAME
    zerosense_
    brne PFA_TICK1
    doliteral_ -13
    rjmp THROW
PFA_TICK1:
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- a-addr )
; Exceptions
; USER variable used by catch/throw
forthword_ HANDLER, 0x8006, "handle"
    call DOUSER
    .word USER_HANDLER

; ( i*x xt -- j*x 0 | i*x n )
; Exceptions
; setup handler to catch exceptions and then EXEC XT.

forthword_ CATCH, 0x8005, "catch"
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
    r_drop_         ; ( ) (R: callerret)
    jmp ZERO

; ( n -- )
; Exceptions
; throw an exception
; assumes jmp to get here
forthword_ THROW, 0x8005, "throw"
    dupzerosense_
    brne PFA_THROW1
	drop_
    ret

PFA_THROW1:
    rcall HANDLER      ; ( n haddr )
    call FETCH         ; ( n RP_handler )
    call RP_STORE      ; ( n ) (R: callerret SP hxt)
    call R_FROM        ; ( n hxt ) (R: callerret SP )
    rcall HANDLER      ; ( n hxt haddr )
    call STORE         ; ( n )
    call R_FROM        ; ( n SP ) (R: callerret )
    call SP_STORE      ; ( n )
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( addr1 n1 c -- addr2 n2 )
; String
; skips leading occurances in string at addr1/n1 leaving addr2/n2 pointing to the 1st non-c character

forthword_ CSKIP, 0x8005, "cskip"
    call TO_B           ; ( -- addr1 n1 ) B: c
PFA_CSKIP1:
    dupzerosense_       ; ( -- addr' n')
    breq PFA_CSKIP2
    call OVER           ; ( -- addr' n' addr' )
    call CFETCH         ; ( -- addr' n' c' )
    call B_             ; ( -- addr' n' c' c )
    call EQUAL          ; ( -- addr' n' f )
    zerosense_          ; ( -- addr' n')
    breq PFA_CSKIP2
    call ONE
    rcall SLASHSTRING
    rjmp PFA_CSKIP1

PFA_CSKIP2:
    ret
; ( -- )
; send Ctrl-Q (XON) to resume transmission
;forthword_ XON, 0x8003, "XON"
;    doliteral_ 17
;    rjmp EMIT

; send Ctrl-S (XOFF) to pause transmission
;forthword_ XOFF, 0x8004, "XOFF"
;    doliteral_ 19
;    rjmp EMIT

; ( -- flag )
; System
; receive a string of at most tibsize characters or cr/lf detected.
; flag will be true if input buffer needs to be processed ie interpreted
; flag will be false if more input needed

forthword_ ACCEPT, 0x8006, "accept"
    ; <begin>
    rcall KEY        ; ( k )
    dupzerosense_    ; ( k )
    brne ACCEPT_haskey
    rjmp ACCEPT_END

ACCEPT_haskey:
    dup_             ; ( k k )
    ; check for EOL
    call TEN         ; ( k k 10 )
    call NOTEQUAL    ; ( k flag )
    ; <if>
    zerosense_       ; ( k )
    brne ACCEPT_checkreturn
    rjmp ACCEPT_END

ACCEPT_checkreturn:
    dup_             ; ( k k )
    doliteral_ 13    ; ( k k 13 )
    call NOTEQUAL    ; ( k flag )
    ; <if>
    zerosense_       ; ( k )
    brne ACCEPT_checkbackspace
    rjmp ACCEPT_END

ACCEPT_checkbackspace:
    ; check backspace
    dup_             ; ( k k )
    doliteral_ 8     ; ( k k 8 )
    call EQUAL       ; ( k f )
    ; <if>
    zerosense_       ; ( k )
    breq ACCEPT_checkcontrol
    ; delete previous character
    ; check beginning of line
    ; if cursor is 0 then at begining of input
    rcall SHARPTIB
    call FETCH               ; ( k #tib )
    ; <if>
    zerosense_               ; ( k )
    brne ACCEPT_dobackspace
    ; at the beginning of the line, ignore this character
    drop_
    jmp ZERO

ACCEPT_dobackspace:
    dup_                 ; ( k k )
    rcall EMIT           ; ( k )
    call SPACE           ; ( k )
    rcall EMIT           ; ( )
    ; decrease cursor position by one
    doliteral_ -1        ; ( -1 )
    rcall SHARPTIB       ; ( -1 #tibaddr )
    call PLUSSTORE       ; ( )
    rjmp ACCEPT_checkmaxinput

ACCEPT_checkcontrol:
    ; check for remaining control characters, replace them with blank
    dup_                 ; ( k k )
    rcall BL             ; ( k k bl )
    call LESS            ; ( k f )
    zerosense_           ; ( k )
    breq ACCEPT_echo
    drop_
    ; replace control key with a space
    rcall BL

ACCEPT_echo:
    ; emit the key
    dup_                 ; ( k k)
    rcall EMIT           ; ( k)
    ; now store the key in the input buffer
    rcall TIB             ; ( k tibaddr )
    rcall SHARPTIB        ; ( k tibaddr #tibaddr )
    call FETCH           ; ( k tibaddr #tib )
    call PLUS             ; ( k tibaddr+#tib ) 
    call CSTORE           ; ( )

    call ONE              ; ( 1 ) 
    rcall SHARPTIB        ; ( 1 #tibaddr )
    call PLUSSTORE        ; ( )

ACCEPT_checkmaxinput:
    ; check if max number of char input
    doliteral_ TIBSIZE    ; ( tibsize )
    rcall SHARPTIB        ; ( tibsize #tibaddr )
    call FETCH            ; ( tibsize #tib)
    call MINUS            ; ( tibsize-#tib)
    ; <if>
    zerosense_            ; ( )
    breq ACCEPT_atmaxinput
    jmp ZERO
    
ACCEPT_atmaxinput:
    ; max number of characters in input buffer so have it processed
    jmp TRUE
    

    ; <then>
ACCEPT_END:
    ret

; ( -- f )
; System
; refills the input buffer
forthword_ REFILL, 0x8006, "refill"
    call DODEFER
    .word USER_REFILL
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( "<spaces>name" -- c )
; Tools
; copy the first character of the next word onto the stack
forthword_ CHAR, 0x8004, "char"
    rcall PARSENAME
    drop_
    jmp CFETCH

; (addr len -- [n] f)
; Numeric IO
; convert a string at addr to a number
forthword_ NUMBER, 0x8006, "number"
    rcall BASE
    call FETCH
    call TO_R               ; ( addr len ) (R: base)
    rcall NUMBERSIGN
    call TO_R               ; ( addr len ) (R: base flagsign)
    rcall PRAEFIX
    rcall NUMBERSIGN         ; ( addr len flagsign2 )
    call R_FROM             ; ( addr len flagsign2 flagsign ) (R: base )
    call OR                 ; ( addr len flagsign' )
    call TO_R               ; ( addr len ) (R: base flagsign')
    call ZERO               ; ( addr len 0 ) starting value
    call RROT               ; ( 0 addr len )
    rcall TO_NUMBER          ; ( n addr' len' )
    ; check length of the remaining string.
    ; if zero: a single cell number is entered
    zerosense_
	breq PFA_NUMBER1

    ; error in string to number conversion
PFA_NUMBER2:
	call TWODROP              ; ( n addr' -- )  (R: base flagsign' )
	r_drop_                   ; ( ) (R: base )
	call ZERO                 ; ( 0 ) (R: base )
    rjmp PFA_NUMBER5

PFA_NUMBER1:
    drop_              ; ( n )  (R: base flagsign' )
    ; incorporate sign into number
    call R_FROM            ; ( n flagsign' ) (R: base )
    zerosense_
    breq PFA_NUMBER4
    rcall NEGATE
PFA_NUMBER4:
    call TRUE              ; ( n true ) (R: base )
PFA_NUMBER5:
    call R_FROM            ; ( n true base ) (R: )
    rjmp BASESTORE         ; ( n true )

; ( addr len -- addr' len' )
; Numeric IO
; skip a numeric prefix character
;forthword_ PRAEFIX, 0x8007, "praefix"
PRAEFIX:        ; ( adr1 len1 -- adr2 len2 )
    call OVER
    call CFETCH
    doliteral_ 0x29
    call GREATER
    zerosense_
    breq PFA_PRAEFIX0
    ret

PFA_PRAEFIX0:
    call OVER
    call CFETCH
    rcall SETBASE
    doliteral_ 0x1
    rjmp SLASHSTRING

; (c -- ) Numeric IO
; R( -- )
; set the BASE value depending on the character
;forthword_ SETBASE, 0x8007, "setbase"
SETBASE:        ; ( c -- )
    dup_
    doliteral_ '$
    call EQUAL
    zerosense_
    breq PFA_SETBASE0
    drop_
    rjmp HEX

PFA_SETBASE0:
    dup_
    doliteral_ '%
    call EQUAL
    zerosense_
    breq PFA_SETBASE1
    drop_
    rjmp BIN

PFA_SETBASE1:
    dup_
    doliteral_ '&
    call EQUAL
    zerosense_
    breq PFA_SETBASE2
    drop_
    rjmp DECIMAL

PFA_SETBASE2:        ; ( error)
    doliteral_ '#
    call EQUAL
    zerosense_
    ret
    rjmp DECIMAL

; (addr len -- addr len flag) Numeric IO
; R( -- )
; check for + - sign
;forthword_ SETBASE, 0x800A, "numbersign"
NUMBERSIGN:        ; ( addr len -- )
    call OVER      ; ( addr len addr )
    call CFETCH    ; ( addr len char )
    doliteral_ '+           ; ( addr len char '+' )
    call EQUAL     ; ( addr len flag )
    zerosense_
    breq PFA_NUMBERSIGN_MINUS
      call ONE
      rcall SLASHSTRING
PFA_NUMBERSIGN_MINUS:
    call OVER          ; ( -- addr len addr )
    call CFETCH
    doliteral_ '-
    call EQUAL         ; ( -- addr len flag )
    dup_               ; ( addr len flag flag )
    call TO_R          ; ( addr len flag ) ( R: flag )
    zerosense_
    breq PFA_NUMBERSIGN_PLUS
    call ONE      ; skip sign character
    rcall SLASHSTRING
PFA_NUMBERSIGN_PLUS:
    call R_FROM        ; ( addr len flag )
    ret

; ( u1 c-addr1 len1 -- u2 c-addr2 len2 )
; Numeric IO
; convert a string to a number  c-addr2/u2 is the unconverted string
forthword_ TO_NUMBER, 0x8007, ">number"
    dupzerosense_
    breq PFA_TO_NUMBER1
        call OVER            ; ( u adr len adr)
        call CFETCH          ; ( u adr len char)
        rcall DIGITQ          ; ( u adr len digit flag)
        zerosense_
        brne PFA_TO_NUMBER2
            ; character is not a recognized number
            ret
PFA_TO_NUMBER2:
        call TO_B            ; ( u adr len) B: digit
        call ROT             ; ( adr len u)
        rcall BASE
        call FETCH           ; ( adr len u base)
        rcall STAR           ; ( adr len u*base)
        call B_              ; ( adr len u' digit)
        call PLUS            ; ( adr len u')
        call RROT            ; ( u' adr len )
        call ONE
        rcall SLASHSTRING
        rjmp TO_NUMBER

PFA_TO_NUMBER1:
    ret

; ( char "ccc<char>" -- c-addr u )
; String
; in input buffer parse ccc delimited string by the delimiter char.
forthword_ PARSE, 0x8005, "parse"
    call TO_R        ; ( -- ) (R: c )
    rcall SRCIN      ; ( -- addr len)
    call R_FROM      ; ( -- addr' len' c)
    rcall CSCAN      ; ( -- addr' len'')
    dup_             ; ( -- addr' len'' len'')
    oneplus_         ; ( -- addr' len'' len''+1 )
    rcall G_IN       ; ( -- addr' len'' len''+1 >in)
    call PLUSSTORE   ; ( -- addr' len'')
    call ONE         ; ( -- addr' len'' 1 )
    rjmp SLASHSTRING ; ( -- addr' len''' )

; ( addr1 n1 c -- addr1 n2 )
; String
; Scan string at addr1/n1 for the first occurance of c, leaving addr1/n2, char at n2 is first non-c character

forthword_ CSCAN, 0x8005, "cscan"
    niptob_           ; ( -- addr1 c ) B: n1
    call OVER         ; ( -- addr1 c addr1 )
    call TO_A         ; ( -- addr1 c ) A: addr1
    call ZERO         ; ( -- addr1 c n2 )
PFA_CSCAN1:
    dup_              ; ( -- addr1 c n2 n2 )
    call B_           ; ( -- addr1 c n2 n2 n1 )
    call LESS         ; ( -- addr1 c n2 flag )
    zerosense_        ; ( -- addr1 c n2 )
    breq PFA_CSCAN3
    call OVER         ; ( -- addr1 c n2 c )
    call ACFETCH      ; ( -- addr1 c n2 c c')
    aplus_
    call EQUAL        ; ( -- addr1 c n2 f)
    zerosense_        ; ( -- addr1 c n2 )
    breq PFA_CSCAN2
    nip_              ; ( -- addr1 n2 )
    ret

PFA_CSCAN2:
    oneplus_          ; ( -- addr1 c n2+1 )
    rjmp PFA_CSCAN1   ; ( -- addr1 c n2+1 )

PFA_CSCAN3:
    call TWODROP      ; ( -- addr1)
    jmp B_            ; ( -- addr1 n1)

; ( -- addr n )
; System
; address and current length of the input buffer
forthword_ SOURCE, 0x8006, "source"
    call DODEFER
    .word USER_SOURCE
    addr_ UDEFERFETCH
    addr_ UDEFERSTORE

; ( addr1 u1 n -- addr2 u2 )
; String
; adjust string from addr1 to addr1+n, reduce length from u1 to u2 by n
forthword_ SLASHSTRING, 0x8002, "/$"
    call OVER    ; ( -- addr1 u1 n u1)
    rcall MIN     ; ( -- addr1 u1 n|u1)
    call ROT     ; ( -- u1 n addr1 )
    call OVER    ; ( -- u1 n addr1 n)
    call PLUS    ; ( -- u1 n addr2 )
    call RROT    ; ( -- addr2 u1 n )
    jmp MINUS   ; ( -- addr2 u2)

; ( -- srcaddr len  )
; String
; Adjust the source addr using >in.
forthword_ SRCIN, 0x8005, "srcin"
    rcall SOURCE         ; ( srcaddr len )
    rcall G_IN           ; ( srcaddr len ginaddr )
    call FETCH           ; ( srcaddr len gin)
    rjmp SLASHSTRING     ; ( srcaddr' len' )


; ( "<name>" -- c-addr u )
; String
; In the SOURCE buffer parse whitespace delimited string. Returns string address within SOURCE.
forthword_ PARSENAME, 0x800A, "parse-name"
    rcall BL
    ; fall thru into SKIPSCANCHAR

; ( c -- addr2 len2 )
; String
; skips char and scan what's left in source for char
;forthword_ SKIPSCANCHAR, 0x800A, "skipscanchar"
SKIPSCANCHAR:
    call TO_R            ; (  ) (R: -- c )
    rcall SRCIN         ; ( srcaddr len )
    call R_FETCH         ; ( srcaddr' len' c )
    rcall CSKIP          ; ( addr2 n2 )
    call R_FROM          ; ( addr2 n2 c )
    rcall CSCAN          ; ( addr2 n3 )

    ; adjust >IN
    call TWOOVER         ; ( addr2 n3 addr2 n3 )
    call PLUS            ; ( addr2 n3 addr2+n3 )
    rcall SOURCE         ; ( addr2 n3 addr2+n3 srcaddr len )
    drop_                ; ( addr2 n3 addr2+n3 srcaddr )
    call MINUS           ; ( addr2 n3 gpos )
    rcall G_IN           ; ( addr2 n3 gpos ginaddr )
    jmp STORE            ; ( addr2 n3 )

; ( addr len --  0 | xt -1 | xt 1 )
; Tools
; search wordlists for the name from string addr/len
forthword_ FINDNAME, 0x8009, "find-name"
    call GET_CURRENT   ; ( addr len EE_FORTHWORDLIST )
    rjmp SEARCH_WORDLIST ; ( -- [xt] flag )

; ( -- )
; System
; check if a ready prompt is required
forthword_ QP_RD, 0x8005, "?p_rd"
    rcall STATE
    call FETCH
    ; output ready prompt if in interpret mode
    ; <-if>
    zerosense_
    brne QP_RD_exit
      rjmp PROMPTRDY
QP_RD_exit:
    ; <then>
      rjmp PROMPTCOMPILE

    

; ( -- )
; System
; main loop - iterate through scheduled tasks 
forthword_ DOTASKS, 0x8007, "dotasks"
    ; <begin>
      rcall DOREFILL
      rcall PAUSE
    ; <again>
    rjmp DOTASKS

; ( -- )
; System
; reset stacks and start over again
forthword_ QUIT, 0x8004, "quit"
    ; reset data stack
    call SP0
    call SP_STORE
    ; reset return stack pointers
    rcall RP0
    call RP_STORE
    ; switch to interpret mode
    call LBRACKET
    
    rjmp DOTASKS
    
; ( -- )
; System
; wait for the input buffer to be refilled then interpret
forthword_ DOREFILL, 0x8008, "dorefill"
  ; if just starting new input line then check prompt and reset input position
  call G_IN               ; ( >inaddr )
  call FETCH              ; ( >in )
  zerosense_              ; ( )
  breq DOREFILL_do
    call ZERO             ; ( 0 )
    dup_                  ; ( 0 0 )
    call G_IN             ; ( 0 0 >inaddr )
    call STORE            ; ( 0 )
    ; reset input buffer cursor position
    rcall SHARPTIB           ; ( 0 #tibaddr )
    call STORE               ; ( )
    rjmp QP_RD
    
DOREFILL_do:
  ; refill the input buffer
  rcall REFILL             ; ( f )
  ; <if>
  zerosense_
  breq DOREFILL_end
  
    ; there is something in the input buffer, try interpreting it      
    rjmp DOINTERPRET
DOREFILL_end:
  ; <then>
  ret      
        
; ( -- )
; System
; interpret what is in the input buffer
forthword_ DOINTERPRET, 0x800B, "dointerpret"
      ; input buffer has something so try to interpret it
      ; setup exception handling in case of error
  doliteral_ pm(INTERPRET)
  rcall CATCH
  ; check to make sure data stack is ok
  call QDUP
  ; <if>
  zerosense_
  breq DOINTERPRET_ok
    dup_
	doliteral_ -2
	call LESS
	; <if>
	zerosense_
	breq DOINTERPRET_reset
	  rcall PROMPTERROR
	
	; <then>
DOINTERPRET_reset:
	rjmp QUIT
	
  ; <then>
DOINTERPRET_ok:
  ; make sure in buf pointer is not at zero
  call ONE                ; ( 1 )
  call G_IN               ; ( 1 >inaddr )
  call PLUSSTORE          ; ( )
  rjmp PROMPTOK

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTRDY, 0x8004, "p_rd"
    rcall CR
    rcall DOSLITERAL
    ascii_ 2, "> "
    rjmp ITYPE

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTCOMPILE, 0x8004, "p_cp"
    rcall CR
    rcall DOSLITERAL
    ascii_ 2, ": "
    rjmp ITYPE

; ( -- )
; System
; send the OK prompt to the command line
forthword_ PROMPTOK, 0x8004, "p_ok"
    rcall DOSLITERAL
    ascii_ 3, " ok"
    rjmp ITYPE

; ( n -- )
; System
; process the error prompt
forthword_ PROMPTERROR, 0x8004, "p_er"
	rcall DOSLITERAL
	ascii_ 4, " ?? "
    rcall ITYPE
    ; save base to return stack
	rcall BASE
	call FETCH
	call TO_R
	; make sure in decimal mode
	rcall DECIMAL
	rcall DOT
	; print the offending word
	rcall G_IN
	call FETCH
	rcall DOT
	; restore base
	call R_FROM
	rjmp BASESTORE

; ( -- )
; Multitasking
; Fetch pause vector and EXEC it. may make a context/task switch
forthword_ PAUSE, 0x8005, "pause"
    call DODEFER
    .word ram_pause
    addr_ FETCH
    addr_ STORE

.pushsection .data
ram_pause: .space 2
.popsection

; ( -- )
; System
; start up amforth.

forthword_ COLD, 0x8004, "cold"
    in_ r10, MCUSR
    clr r11
    clr zerol
    clr zeroh
    out_ MCUSR, zerol
    ; init first user data area
    ; allocate space for User Area
    ldi ZL, lo8(ram_user1)
    ldi ZH, hi8(ram_user1)
    movw upl, ZL
    ; init return stack pointer
	; using the MCU SP registers
    ldi temp0, lo8(rstackstart)
    out_ SPL,temp0
    std Z+USER_RP, temp0
    ldi temp1, hi8(rstackstart)
    out_ SPH,temp1
    std Z+USER_RP+1, temp1

    ; init parameter stack pointer
    ldi YL, lo8(stackstart)
    std Z+USER_SP0, YL
    ldi YH, hi8(stackstart)
    std Z+USER_SP0+1, YH
    rjmp WARM

.pushsection .data
ram_user1: .space SYSUSERSIZE + APPUSERSIZE
.popsection

; ( nx* -- ) (R: ny* -- )
; System
; initialize asforth further. EXECs turnkey operation and go to quit
forthword_ WARM, 0x8004, "warm"
    rcall INITUSER
    doliteral_ pm(NOOP)
    doliteral_ pm(PAUSE)
    rcall DEFERSTORE
    rcall TURNKEY
    rjmp QUIT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr)
; Stack
; start address of the data stack
forthword_ SP0, 0x8003, "sp0"
    call DOVALUE
    .word USER_SP0
    addr_ FETCHU
    addr_ STOREU

; ( -- addr)
; Stack
; address of user variable to store top-of-stack for inactive tasks
forthword_ SP, 0x8002, "sp"
    call DOUSER
    .word USER_SP

; ( -- addr)
; Stack
; start address of return stack
forthword_ RP0, 0x8003, "rp0"
    rcall DORP0
    jmp FETCH

; ( -- addr)
; Stack
; user variable of the address of the initial return stack
;forthword_ DORP0, 0x8005, "(rp0)"
DORP0:
    call DOUSER
    .word USER_RP

; ( -- n )
; Stack
; number of single-cell values contained in the data stack before n was placed on the stack.
forthword_ DEPTH, 0x8005, "depth"
    call SP0
    call SP_FETCH
    call MINUS
    twoslash_
    oneminus_
    ret

; ( -- ) (R: i*x - j*x )
; System
; interpret input word by word.
forthword_ INTERPRET, 0x8009, "interpret"
    ; begin
    rcall PARSENAME     ; ( -- addr len )

    ; ?while
    dupzerosense_       ; ( -- addr len )
    breq INTERPRET_END

    rcall RECX          ; ( )
    rcall QSTACK        ; ( )

    ; repeat
    rjmp INTERPRET

INTERPRET_END:
    jmp TWODROP

; ( addr len -- )
; System
; recognize and execute name of word in ram using recognizer list.
forthword_ RECX, 0x8004, "recx"
    call ZERO            ; ( addr len n )
    call TO_R            ; ( addr len ) (R: n )

    ; begin
RECX_BEGIN:
    call R_FETCHPLUS     ; ( addr len n ) (R: n+1 )
    rcall RECFETCH       ; ( addr len recxt )

    ; ?while
    dupzerosense_        ; ( addr len recxt )
    breq RECX_DONE

    call TO_R            ; ( addr len ) (R: n+1 recxt )
    call TWOOVER         ; ( addr len addr len  )
    call R_FROM          ; ( addr len addr len recxt ) (R: n )
    call RROT            ; ( addr len recxt addr len )
    call TO_R            ; ( addr len recxt addr ) (R: n len )
    call TO_R            ; ( addr len recxt ) (R: n len addr )
    call EXEC            ; ( flag )
    call R_FROM          ; ( flag addr ) (R: n len )
    call R_FROM          ; ( flag addr len ) (R: n )
    call ROT             ; ( addr len flag )

    ; <if>
    zerosense_           ; ( addr len )
    breq RECX_NEXT

    call TWODROP         ; ( ) (R: n)
    r_drop_              ; ( ) (R: )
    ret

    ; <then>
    ; <repeat>
RECX_NEXT:
    rjmp RECX_BEGIN

RECX_DONE:
    call TWODROP
    drop_
    r_drop_
    ret

; ( n -- recxt )
; Interpreter
; get a recognizer from the recognizer list
; recxt is zero if at end of list
forthword_ RECFETCH, 0x8004, "rec@"
    dup_                            ; ( n n )
    doliteral_ EE_RECOGNIZERLISTLEN ; ( n n ee-reclen )
    call FETCHE                     ; ( n n len )
    call LESS                       ; ( n flag )
    ; <if>
    zerosense_                      ; ( n )
    breq PFA_RECFETCH_ZERO
      twostar_                        ; ( 2*n )
      doliteral_ EE_RECOGNIZERLIST    ; ( 2*n listaddr )
      call PLUS                       ; ( listaddr' )
      jmp FETCHE                      ; ( recxt )

    ; <then>
PFA_RECFETCH_ZERO:
    zerotos_
    ret





; ( addr len -- n f )
; Interpreter
; recognizer for integer numbers
forthword_ REC_NUM, 0x8007, "rec-num"
    ; try converting to a number
    rcall NUMBER             ; ( [n] f )
    zerosense_
    breq PFA_REC_NONUMBER

    rcall STATE
    call FETCH              ; ( n state )
    zerosense_
    breq PFA_REC_NUMBER_OK
    rcall LIT
PFA_REC_NUMBER_OK:
    jmp TRUE               ; ( n true )

PFA_REC_NONUMBER:
    jmp ZERO               ; ( 0 )

; ( addr len -- f )
; Interpreter
; recognize a word in the dictionary
; will 
forthword_ REC_WORD, 0x8008, "rec-word"

    rcall FINDNAME
    dupzerosense_
    brne REC_WORD_FOUND
    ret

REC_WORD_FOUND:
	; either compile or EXEC the XT
    ; check if word is immediate: bit 7 is clear if immediate
	sbrs tosh, 7 ; skip next instruction if bit 7 is set: not immediate word
	    ; flag is 0: always EXEC
	    rjmp REC_WORD_EXECUTE

REC_WORD_CHECKSTATE:
        ; check state
	    rcall STATE
	    call FETCH
	    zerosense_
	    breq REC_WORD_EXECUTE
        ; in compile mode so compile xt
 	    rcall COMPILEXT
	    rjmp REC_WORD_OK
REC_WORD_EXECUTE:
    drop_
    ; state is zero, EXEC xt
    call EXEC
REC_WORD_OK:
    jmp TRUE

; ( addr len --  )
; Interpreter
; recognizer for NOT FOUND
forthword_ REC_UNK, 0x8007, "rec-unk"
    rcall TYPE
    doliteral_ -13
    rjmp THROW


; ( --  )
; Tools
; check stack underflow, throw exception -4
forthword_ QSTACK, 0x8006, "?stack"
; : ?stack ( -- )
;    depth 0< if -&4 throw then
; ;
    rcall DEPTH
    call LESSZERO
    zerosense_
    brne PFA_QSTACKERROR
    ret
PFA_QSTACKERROR:
      doliteral_ -4
      rjmp THROW

; ( -- )
; Tools
; print the version string
forthword_ VER, 0x8003, "ver"
    call ENV_FORTHNAME
    rcall ITYPE
    call SPACE
    rcall BASE
    call FETCH
    call TO_R
    rcall DECIMAL

    call ENV_FORTHVERSION
    rcall L_SHARP
    rcall SHARP
    doliteral_ '.
    rcall HOLD
    rcall SHARP_S
    rcall SHARP_G
    call R_FROM
    rcall BASESTORE

    rcall TYPE
    call SPACE
    call ENV_CPU
    rjmp ITYPE



; ( -- n )
; Tools
; Amount of available RAM (incl. PAD)
forthword_ UNUSED, 0x8006, "unused"
    call SP0
    rcall HERE
    jmp MINUS

; ( n <name> -- )
; Compiler
; create a dictionary entry for a value and allocate 1 cell in EEPROM.
forthword_ VALUE, 0x8003, "val"
    rcall DOCREATE
    call GET_CURRENT
    call STOREE

    rcall COMPILE
    addr_ DOVALUE
    rcall EDP
    dup_
    rcall COMMA
    dup_
    twoplus_
    rcall DOTO
    .word EDP
    call STOREE
    rcall COMPILE
    addr_ FETCHE
    rcall COMPILE
    addr_ STOREE
    call EXIT

; ( n <name> -- )
; Tools
; store the TOS to the named value (eeprom cell)
forthword_ TO, 0x0002, "to"
    rcall TICK          ; ( n xt ) get address of next word from input stream
    rcall STATE
    call FETCH
    zerosense_
    breq PFA_TO1        ; not in compile state, so jump to runtime operation
    rcall COMPILE
    addr_ DOTO
    rjmp COMMA

; ( n -- ) (R: IP -- IP+2)
; Tools
; runtime portion of to
;forthword_ DOTO, 0x8004, "(to)"
;must be called
DOTO:
    call R_FETCHPLUS    ; ( n -- n raddr ) (R: raddr -- raddr+1 )
    ; next cell contains address of deferred word
    call FETCHI         ; ( n defxt )
PFA_TO1:
    rjmp DEFERSTORE

; ( addr -- xt2 )
; System
; does the real defer@ for user based defers
;forthword_ UDEFERFETCH, 0x8007, "Udefer@"
UDEFERFETCH:
    call UP_FETCH
    call PLUS
    jmp FETCH

; ( xt1 xt2 -- )
; System
; does the real defer! for user based defers
;forthword_ UDEFERSTORE, 0x8007, "Udefer!"
UDEFERSTORE:
    oneplus_
    call FETCHI
    call UP_FETCH
    call PLUS
    jmp STORE

; ( xt1 xt2 -- )
; System
; stores xt1 as the xt to be EXECd when xt2 is called
forthword_ DEFERSTORE, 0x8006, "defer!"
    ; skip past call to get to data
    twoplus_                 ; ( xt1 xt2+2 )
    dup_                     ; ( xt1 xt2+2 xt2+2 )
    ; get defer address
    call FETCHI              ; ( xt1 xt2+2 defaddr )
    call SWAP                ; ( xt1 defaddr xt2+2 )
    ; skip past data address and deferfetch section to get
    ; to deferstore section
    twoplus_                 ; ( xt1 defaddr xt2+4 )
    ; get defered store xt address
    call FETCHI              ; ( xt1 defaddr xtstore )
    jmp EXEC

; ( xt1 -- xt2 )
; System
; returns the XT associated with the given XT
forthword_ DEFERFETCH, 0x8006, "defer@"
    ; skip passed call to get to data field
    twoplus_                 ; ( xt1+2 )
    dup_                     ; ( xt1+2 xt1+2 )
    ; skip past data to get to defer fetch field
    oneplus_                 ; ( xt1+2 xt1+3 )
    call FETCHI              ; ( xt1+2 deferfetchxt )
    jmp EXEC

; ( i*x -- j*x )
; System
; runtime of defer
;forthword_ DODEFER, 0x800a, "(defer)"
DODEFER:
    ; top of return stack has return address of caller
    ; return address is data for defer so pop it onto data stack
    ; will not be returning to the caller
    call R_FROM         ; ( Raddr ) (R: Raddr -- )
    call DEFX           ; ( XTdefer )
    ; EXEC the defer
    jmp EXEC

; ( c-addr len wid -- [ 0 ] | [ xt xtflags] )
; Search Order
; searches the word list wid for the word at c-addr/len
forthword_ SEARCH_WORDLIST, 0x800f, "search-wordlist"
    call FETCHE                ; ( c-addr len nfa )
PFA_SEARCH_WORDLIST0:
    dupzerosense_              ; ( c-addr len nfa )
    brne PFA_SEARCH_WORDLIST1
      ; terminating, 0 found
      call TWODROP             ; ( c-addr )
      zerotos_                 ; ( 0 )
      ret

PFA_SEARCH_WORDLIST1:
    ; <begin>
    call TO_R                  ; ( c-addr len ) (R: nfa )
    call TWOOVER               ; ( c-addr len c-addr len )
    call R_FETCH               ; ( c-addr len c-addr len nfa )
    rcall ICOUNT               ; ( c-addr len c-addr len f-addr len )
    zerohigh_                  ; ( c-addr len c-addr len f-addr 0:len )
    rcall ICOMPARE             ; ( c-addr len flag )
    zerosense_
    breq PFA_SEARCH_WORDLISTNEXT
      ; we found the string
      call TWODROP
      call R_FROM ; ( -- iaddr )
      dup_
      call FETCHI
      call TO_R
      call NFA2LFA
      oneplus_  ; lfa2xt
      ;call ONE
      call R_FROM ; check flags
      ;call LESSZERO ; change here for multiple flags in header
      ;zerosense_
      ;breq PFA_SEARCH_WORDLISTIMMEDIATE
      ;rcall NEGATE

PFA_SEARCH_WORDLISTIMMEDIATE:
      ret

PFA_SEARCH_WORDLISTNEXT:
      ; next try
      call R_FROM
      call NFA2LFA
      call FETCHI
      rjmp PFA_SEARCH_WORDLIST0

; ( r-addr r-len f-addr f-len --  f)
; Memory
; compares string in RAM with string in flash
forthword_ ICOMPARE, 0x8008, "icompare"
    call TO_B               ; ( r-addr r-len f-addr)  B: f-len
    call OVER               ; ( r-addr r-len f-addr r-len)
    call B_                 ; ( r-addr r-len f-addr r-len f-len )
    ; check if strings are same length
    call NOTEQUAL           ; ( r-addr r-len f-addr flag )
    ; if
    zerosense_              ; ( r-addr r-len f-addr )
    breq PFA_ICOMPARE_SAMELEN

    ; strings are not the same length
    call TWODROP            ; ( r-addr )
    zerotos_                ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_SAMELEN:
    twostar_                ; ( r-addr len f-addr*2 )
    call TO_A               ; ( r-addr len ) A: f-addr
    oneplus_                ; ( r-addr len+1 )
    twoslash_               ; ( r-addr (len+1)/2 )

    ; begin
PFA_ICOMPARE_LOOP:
    copytob_                ; ( r-addr len ) B: len
    ; while
    zerosense_              ; ( r-addr )
    breq PFA_ICOMPARE_DONE

    dup_                    ; ( r-addr r-addr )
    call FETCH              ; ( r-addr r-cc )
    call AFETCHI            ; ( r-addr r-cc f-cc )
    aplus_
    ; flash strings are zero-padded at the last cell
    ; that means: if the flash cell is less $0100, than mask the
    ; high byte in the ram cell
    dup_                    ; ( r-addr r-cc f-cc f-cc )
    doliteral_ 0x100        ; ( r-addr r-cc f-cc f-cc 0x100 )
    call ULESS               ; ( r-addr r-cc f-cc flag)
    ; if
    zerosense_
    breq PFA_ICOMPARE_LASTCELL
    call SWAP               ; ( r-addr f-cc r-cc)
    zerohigh_               ; ( r-addr f-cc 0:r-cc)
    ; then
PFA_ICOMPARE_LASTCELL:
    call NOTEQUAL           ; ( r-addr flag)
    ; if
    zerosense_              ; ( r-addr )
    breq PFA_ICOMPARE_NEXTLOOP

    ; strings don't match
    zerotos_                ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_NEXTLOOP:
    twoplus_              ; ( r-addr+2 )
    call B_               ; ( r-addr+2 len )
    oneminus_             ; ( r-addr+2 len-1 )

    ; repeat
    rjmp PFA_ICOMPARE_LOOP

PFA_ICOMPARE_DONE:
    ; strings are the same
    drop_
    doliteral_ -1
    ret

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ STAR, 0x8001, "*"
    rcall MSTAR
    drop_
    ret

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ MSTAR, 0x8002, "m*"
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
    sbc   temp5, zeroh
    add   tosh,  r0
    adc   temp4, r1
    adc   temp5, zeroh

    ; signed al*bh
    mulsu temp1, temp2
    sbc   temp5, zeroh
    add   tosh,  r0
    adc   temp4, r1
    adc   temp5, zeroh

    pushtos
    movw tosl, temp4
    ret

; ( ud u2 -- rem quot)
; Arithmetics
; unsigned division ud / u2 with remainder
forthword_ UMSLASHMOD, 0x8006, "um/mod"
    movw temp4, tosl

    ld temp2, Y+
    ld temp3, Y+

    ld temp0, Y+
    ld temp1, Y+

;; unsigned 32/16 -> 16r16 divide

PFA_UMSLASHMODmod:

  ; set loop counter
    ldi temp6, 0x10

PFA_UMSLASHMODmod_loop:
    ; shift left, saving high bit
    clr temp7
    lsl temp0
    rol temp1
    rol temp2
    rol temp3
    rol temp7

  ; try subtracting divisor
    cp temp2, temp4
    cpc temp3, temp5
    cpc temp7,zerol

    brcs PFA_UMSLASHMODmod_loop_control

PFA_UMSLASHMODmod_subtract:
    ; dividend is large enough
    ; do the subtraction for real
    ; and set lowest bit
    inc temp0
    sub temp2, temp4
    sbc temp3, temp5

PFA_UMSLASHMODmod_loop_control:
    dec  temp6
    brne PFA_UMSLASHMODmod_loop

PFA_UMSLASHMODmod_done:
    ; put remainder on stack
    st -Y,temp3
    st -Y,temp2

    ; put quotient on stack
    movw tosl, temp0
    ret

; ( n1 n2 n3 -- rem quot)
; Arithmetics
; signed multiply n1 * n2 and division  with n3 with double precision intermediate and remainder
forthword_ STARSLASHMOD, 0x8005, "*/mod"
    call TO_R
    rcall MSTAR
    call R_FROM
    rjmp UMSLASHMOD

; (n1 n2 n3 -- n4)
; Arithmetics
; signed multiply and division with double precision intermediate
forthword_ STARSLASH, 0x8002, "*/"
    rcall STARSLASHMOD
    nip_
    ret

; (addr-from addr-to n -- )
; Memory
; copy data in RAM, from lower to higher addresses

forthword_ CMOVE, 0x8005, "cmove"
    ld ZL, Y+
    ld ZH, Y+ ; addr-to
    ld XL, Y+
    ld XH, Y+ ; addr-from
    mov temp0, tosh
    or temp0, tosl
    brbs 1, PFA_CMOVE1
PFA_CMOVE2:
    ld temp1, X+
    st Z+, temp1
    oneminus_
    brbc 1, PFA_CMOVE2
PFA_CMOVE1:
    poptos
    ret

; (start dest count -- )
; Memory
; copy flash memory.  Assumes no overlap.
forthword_ ITOI, 0x8004, "itoi"
    niptoa_         ; ( start count ) A: dest
ITOI_B:
    dupzerosense_   ; ( start count )
    breq ITOI_END
    call TO_B       ; ( start ) B: count
    dup_            ; ( start start )
    call FETCHI     ; ( start val )
    call A_         ; ( start val dest )
    call STOREI     ; ( start )
    aplus_
    oneplus_        ; ( start+1 )
    call B_         ; ( start+1 count )
    oneminus_       ; ( start+1 count-1 )
    rjmp ITOI_B

ITOI_END:
    jmp TWODROP

; ( start count -- )
; compiler
; copy code to current dictionary entry
forthword_ INLINE, 0x8006, "inline"
    ; set disable tail call optimize flag:
    sbr fflags, 1<<DIS_CALL_OPT_FB

    ; dp swap dup dp+ itoi
    call DP
    call SWAP
    dup_
    rcall DPPLUS
    rjmp ITOI



; ( -- )
; System
; Arduino pin 13 (portB bit 5) debug.

forthword_ DBGOFF, 0x8004, "dbg-"
	sbi 4,5
	cbi 5,5
    ret

forthword_ DBGON, 0x8004, "dbg+"
	sbi 4,5
	sbi 5,5
    ret

; ( -- addr )
; System Variable
; terminal input buffer address
forthword_ TIB, 0x8003, "tib"
    doliteral_ ram_tib
    ret

.pushsection .data
ram_tib: .space TIBSIZE
.popsection

; ( -- f )
; System
; refills the input buffer - flag is true if buffer has something
forthword_ REFILLTIB, 0x800a, "refill-tib"
REFILLTIB_begin:
    rcall ACCEPT               ; ( f )
    dupzerosense_              ; ( f )
    breq REFILLTIB_exit
    ;doliteral_ -1
    jmp CR
    
REFILLTIB_exit:
    ret
    ;jmp ZERO

; ( -- addr n )
; System
; address and current length of the input buffer
forthword_ SOURCETIB, 0x800A, "source-tib"
    rcall TIB
    call SHARPTIB
    jmp FETCH

; ( -- v)
; System Value
; address of the default user area content in eeprom
forthword_ EEUSER, 0x8007, "ee-user"
  doliteral_ EE_INITUSER
  ret

; ( e-addr r-addr len -- )
; Memory
; copy len cells from eeprom to ram
forthword_ EE2RAM, 0x8006, "ee>ram"
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
forthword_ INITUSER, 0x8009, "init-user"
    rcall EEUSER
    call UP_FETCH
    doliteral_ SYSUSERSIZE
    twoslash_
    rjmp EE2RAM
