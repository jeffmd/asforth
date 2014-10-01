;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; ( -- a-addr )
; Numeric IO
; location of the cell containing the number conversion radix

forthword_ BASE, 0, 4, "base"
    call DOUSER
    addr_ USER_BASE



; ( -- addr )
; System Variable
; variable holding the number of characters in TIB
forthword_ SHARPTIB, 0, 4, "#tib"
    doliteral_ ram_sharptib
    ret

.pushsection .noinit
ram_sharptib: .space 2
.popsection

; ( -- a-addr )
; System Variable
; Address of the temporary scratch buffer.

forthword_ PAD, 0, 3, "pad"
    rcall HERE
    doliteral_ 100
    jmp PLUS

; ( c -- )
; Character IO
; fetch the emit vector and EXEC it. should emit a character from TOS
forthword_ EMIT, 0, 4, "emit"
    call DODEFER
    .word USER_EMIT
    addr_ FETCHU
    addr_ STOREU

; ( -- f )
; Character IO
; fetch emit? vector and EXEC it. should return the ready-to-send condition
forthword_ EMITQ, 0, 5, "emit?"
    call DODEFER
    .word USER_EMITQ
    addr_ FETCHU
    addr_ STOREU

; ( -- c )
; Character IO
; fetch key vector and EXEC it, should leave a single character on TOS
forthword_ KEY, 0, 3, "key"
    call DODEFER
    .word USER_KEY
    addr_ FETCHU
    addr_ STOREU

; ( -- f)
; Character IO
; fetch key? vector and EXEC it. should turn on key sender, if it is disabled/stopped
forthword_ KEYQ, 0, 4, "key?"
    call DODEFER
    .word USER_KEYQ
    addr_ FETCHU
    addr_ STOREU


; ( -- e-addr )
; System Value
; address of the next free address in eeprom
forthword_ EDP, 0, 3, "edp"
    call DOVALUE
    .word EE_EDP
    addr_ FETCHE
    addr_ STOREE

; ( -- addr )
; System Value
; address of the next free data space (RAM) cell
forthword_ HERE, 0, 4, "here"
    call DOVALUE
    .word EE_HERE
    addr_ FETCHE
    addr_ STOREE

; ( n -- )
; System
; allocate or release memory in RAM

forthword_ ALLOT, 0, 5, "allot"
    rcall HERE
    call PLUS
    rcall DOTO
    addr_ HERE
    ret


; ( distance -- distance flag )
; Tools
; Determines if a relative distance is within a relative jmp/call range
; flag is true if relative distance is between -2048 and 2047
forthword_ QRDIST, 0, 6, "?rdist"
    cpi tosh, 0x08
    brsh QDIST_HTEST
QDIST_INRANGE:
    jmp TRUE
QDIST_HTEST:
    cpi tosh, 0xf8
    brsh QDIST_INRANGE
    jmp ZERO

; ( n -- )
; Numeric IO
; save base
forthword_ BASESTORE, 0, 5, "base!"
    rcall BASE
    jmp STORE

; ( n -- )
; Numeric IO
; load base
forthword_ BASEFETCH, 0, 5, "base@"
    rcall BASE
    jmp FETCH

; ( -- )
; Numeric IO
; set base for number conversion to 2

forthword_ BIN, 0, 3, "bin"
    call TWO
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for numeric conversion to 10
forthword_ DECIMAL, 0, 7, "decimal"
    call TEN
    rjmp BASESTORE

; ( -- )
; Numeric IO
; set base for number conversion to 16
forthword_ HEX, 0, 3, "hex"
    doliteral_ 16
    rjmp BASESTORE
    
; ( -- 32 )
; Character IO
; put ascii code of the blank/space character to the stack

forthword_ BL, 0, 2, "bl"
    doliteral_ 32
    ret

; ( -- n*y )
; System Value
; Deferred action during startup/reset

forthword_ TURNKEY, 0, 7, "turnkey"
    call DODEFER
    .word EE_TURNKEY
    addr_ FETCHE
    addr_ STOREE


; ( n1 n2 -- n1|n2 )
; Compare
; compare two values leave the smaller one
forthword_ MIN, 0, 3, "min"
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
forthword_ MAX, 0, 3, "max"
    call TWOOVER
    call LESS
    rjmp MIN_TEST

;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr )
; Numeric IO
; pointer to current write position in the Pictured Numeric Output buffer
forthword_ HLD, 0, 3, "hld"
    doliteral_ ram_hld
    ret

.pushsection .noinit
ram_hld: .space 2
.popsection

; ( c -- )
; Numeric IO
; prepend character to pictured numeric output buffer

forthword_ HOLD, 0, 4, "hold"
    doliteral_ -1
    rcall HLD    
    call PLUSSTORE
    rcall HLD
    call FETCH
    jmp CSTORE   

; ( -- )
; Numeric IO
; initialize the pictured numeric output conversion process
forthword_ L_SHARP, 0, 2, "<#"
    rcall PAD
    rcall HLD
    jmp STORE

; ( u1 -- u2 )
; Numeric IO
; pictured numeric output: convert one digit
forthword_ SHARP, 0, 1, "#"
    rcall BASEFETCH     ; ( u1 base )
    call USLASHMOD      ; ( rem u2 )
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
    doliteral_ 0x30     ; ( u2 char 0x30 )
    call PLUS           ; ( u2 char )
    rjmp HOLD           ; ( u2 )

; ( u -- 0 )
; Numeric IO
; pictured numeric output: convert all digits until 0 (zero) is reached
forthword_ SHARP_S, 0, 2, "#s"
    ; <begin>
    rcall SHARP          ; ( u )
    ; <until>
    dupzerosense_
    brne SHARP_S
    ret

; ( u1 -- addr count )
; Numeric IO
; Pictured Numeric Output: convert PNO buffer into an string
forthword_ SHARP_G, 0, 2, "#>"
    drop_
    rcall HLD
    call FETCH
    rcall PAD
    call OVER
    jmp MINUS

; ( n -- )
; Numeric IO
; place a - in HLD if n is negative
forthword_ SIGN, 0, 4, "sign"
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
forthword_ DOT, 0, 1, "."
    dup_             ; ( n n )
    rcall ABS        ; ( n n' )
    call ZERO        ; ( n n' 0 ) not right aligned
    rcall DOTR
    rjmp SPACE
; : .         ( s -- )    .r ;

; ( wantsign n w -- )
; Numeric IO
; singed PNO with cell numbers, right aligned in width w
forthword_ DOTR, 0, 2, ".r"
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
forthword_ DIGITQ, 0, 6, "digit?"
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
forthword_ ULESSEQUAL, 0, 3, "u<="
    call UGREATER
    not_
    ret

; ( u1 u2 -- flag )
; Compare
; compare two unsigned numbers, returns true flag if u1 is greater then or equal to u2
forthword_ UGREATEREQUAL, 0, 3, "u>="
    call ULESS
    not_
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr len )
; String
; runtime portion of sliteral
; string literal is located starting at return cell
; return cell is string length
; string then occupies the following cells: c2c1 c4c3 ...

forthword_ DOSLITERAL, 0, 10, "(sliteral)"

  ; get return address to get string length, calculate string address start,
  ; and calculate return address
  call R_FROM   ; ( -- raddr )  (R: raddr -- )
  dup_          ; ( -- raddr raddr )
  dup_          ; ( -- raddr raddr raddr )
  ; get the string length
  call FETCHI   ; ( -- raddr raddr slength )
  call SWAP     ; ( -- raddr slength raddr )
  ; calculate string address
  oneplus_      ; ( -- raddr slength straddr )
  ; calculate the number of words to skip over for return address
  call FLIP     ; ( -- straddr slength raddr )
  call OVER     ; ( -- straddr slength raddr slength)
  call WALIGN   ; ( -- straddr slength raddr k/2 )
  call PLUS     ; ( -- straddr slength raddr+k/2 )
  ; also skip string length
  oneplus_      ; ( -- straddr slength raddr+k/2+1 )
  call TO_R     ; ( -- )
  ret


; ( addr n --  )
; Tools
; reads string from flash and prints it
forthword_ ITYPE, 0, 5, "itype"
    call BTOW        ; ( addr len/2 rem )
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
    highbyte_             ; ( len c2c1 )
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
forthword_ ICOUNT, 0, 2, "$l"
    dup_
    oneplus_
    call SWAP
    jmp FETCHI

; ( -- )
; Character IO
; cause subsequent output appear at the beginning of the next line
forthword_ CR, 0, 2, "cr"
    doliteral_ 13
    rcall EMIT
    call TEN
    rjmp EMIT

; ( -- )
; Character IO
; emits a space (bl)
forthword_ SPACE, 0, 5, "space"
    rcall BL
    rjmp EMIT

; ( n -- )
; Character IO
; emits n space(s) (bl)
; only accepts positive values
forthword_ SPACES, 0, 6, "spaces"
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
forthword_ TYPE, 0, 4, "type"
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



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- a-addr )
; Exceptions
; USER variable used by catch/throw
forthword_ HANDLER, 0, 6, "handle"
    call DOUSER
    .word USER_HANDLER

; ( i*x xt -- j*x 0 | i*x n )
; Exceptions
; setup handler to catch exceptions and then EXEC XT.

forthword_ CATCH, 0, 5, "catch"
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
forthword_ THROW, 0, 5, "throw"
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
    jmp SP_STORE      ; ( n )

; ( -- )
; send Ctrl-Q (XON) to resume transmission
;forthword_ XON, 0, 3, "XON"
;    doliteral_ 17
;    rjmp EMIT

; send Ctrl-S (XOFF) to pause transmission
;forthword_ XOFF, 0, 4, "XOFF"
;    doliteral_ 19
;    rjmp EMIT

; ( -- flag )
; System
; receive a string of at most tibsize characters or cr/lf detected.
; flag will be true if input buffer needs to be processed ie interpreted
; flag will be false if more input needed

forthword_ ACCEPT, 0, 6, "accept"
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

    ;call ONE              ; ( 1 ) 
    rcall SHARPTIB        ; ( 1 #tibaddr )
    call ONEPLUSSTORE        ; ( )

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
forthword_ REFILL, 0, 6, "refill"
    call DODEFER
    .word USER_REFILL
    addr_ FETCHU
    addr_ STOREU

; ( "<spaces>name" -- c )
; Tools
; copy the first character of the next word onto the stack
forthword_ CHAR, 0, 4, "char"
    call PNAME
    drop_
    jmp CFETCH

; (addr len -- [n] f)
; Numeric IO
; convert a string at addr to a number
forthword_ NUMBER, 0, 6, "number"
    rcall BASEFETCH
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
	nip_                      ; ( addr' )  (R: base flagsign' )
	r_drop_                   ; ( addr' ) (R: base )
	zerotos_                  ; ( 0 ) (R: base )
    rjmp PFA_NUMBER5

PFA_NUMBER1:
    drop_              ; ( n )  (R: base flagsign' )
    ; incorporate sign into number
    call R_FROM            ; ( n flagsign' ) (R: base )
    zerosense_
    breq PFA_NUMBER4
    call NEGATE
PFA_NUMBER4:
    call TRUE              ; ( n true ) (R: base )
PFA_NUMBER5:
    call R_FROM            ; ( n true base ) (R: )
    rjmp BASESTORE         ; ( n true )

; ( addr len -- addr' len' )
; Numeric IO
; skip a numeric prefix character
;forthword_ PRAEFIX, 0, 7, "praefix"
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
    jmp SLASHSTRING

; (c -- ) Numeric IO
; R( -- )
; set the BASE value depending on the character
;forthword_ SETBASE, 0, 7, "setbase"
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
    ;doliteral_ '#
    ;call EQUAL
    ;zerosense_
    ;ret
    rjmp DECIMAL

; (addr len -- addr len flag) Numeric IO
; R( -- )
; check for + - sign
;forthword_ SETBASE, 0, A, "numbersign"
NUMBERSIGN:        ; ( addr len -- )
    call OVER      ; ( addr len addr )
    call CFETCH    ; ( addr len char )
    doliteral_ '+  ; ( addr len char '+' )
    call EQUAL     ; ( addr len flag )
    zerosense_
    breq PFA_NUMBERSIGN_MINUS
      call ONE
      call SLASHSTRING
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
    call SLASHSTRING
PFA_NUMBERSIGN_PLUS:
    call R_FROM        ; ( addr len flag )
    ret

; ( u1 c-addr1 len1 -- u2 c-addr2 len2 )
; Numeric IO
; convert a string to a number  c-addr2/u2 is the unconverted string
forthword_ TO_NUMBER, 0, 7, ">number"
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
    

; ( -- )
; System
; start up asforth.

forthword_ COLD, 0, 4, "cold"
    in_ r10, MCUSR
    clr r11
    clr zerol
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

.pushsection .noinit
ram_user1: .space SYSUSERSIZE + APPUSERSIZE
.popsection

; ( nx* -- ) (R: ny* -- )
; System
; initialize asforth further. EXECs turnkey operation and go to quit
forthword_ WARM, 0, 4, "warm"
    rcall INITUSER
    doliteral_ pm(NOOP)
    doliteral_ pm(PAUSE)
    rcall DEFERSTORE
    ; init DP in ram from eeprom
    call DPFETCHE
    call ONLY
    rcall USART
    rcall INTON
    rcall TURNKEY
    jmp QUIT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( -- addr)
; Stack
; start address of the data stack
forthword_ SP0, 0, 3, "sp0"
    call DOVALUE
    .word USER_SP0
    addr_ FETCHU
    addr_ STOREU

; ( -- addr)
; Stack
; address of user variable to store top-of-stack for inactive tasks
forthword_ SP, 0, 2, "sp"
    call DOUSER
    .word USER_SP

; ( -- addr)
; Stack
; start address of return stack
forthword_ RP0, 0, 3, "rp0"
    rcall DORP0
    jmp FETCH

; ( -- addr)
; Stack
; user variable of the address of the initial return stack
;forthword_ DORP0, 0, 5, "(rp0)"
DORP0:
    call DOUSER
    .word USER_RP



; ( -- n )
; Stack
; number of single-cell values contained in the data stack before n was placed on the stack.
forthword_ DEPTH, 0, 5, "depth"
    rcall SP0
    call SP_FETCH
    call MINUS
    twoslash_
    oneminus_
    ret

; ( --  )
; Tools
; check stack underflow, throw exception -4
forthword_ QSTACK, 0, 6, "?stack"
; : ?stack ( -- )
;    depth 0< if -&4 throw then
; 
    rcall DEPTH
    call LESSZERO
    zerosense_
    brne PFA_QSTACKERROR
    ret
PFA_QSTACKERROR:
      doliteral_ -4
      rjmp THROW


; ( n <name> -- )
; Tools
; store the TOS to the named value (eeprom cell)
forthword_ TO, IMMEDIATE_EN, 2, "to"
    rcall TICK          ; ( n xt ) get address of next word from input stream
    call STATE
    call FETCH
    zerosense_
    breq PFA_TO1        ; not in compile state, so jump to runtime operation
    call DOCOMPILE
    addr_ VE_DOTO
    jmp COMMA

; ( n -- ) (R: IP -- IP+2)
; Tools
; runtime portion of to
; must be called
forthword_ DOTO, DIS_CALL_OPT, 4, "(to)"
    call R_FETCHPLUS    ; ( n -- n raddr ) (R: raddr -- raddr+1 )
    ; next cell contains address of deferred word
    call FETCHI         ; ( n defxt )
PFA_TO1:
    rjmp DEFERSTORE



; ( xt1 xt2 -- )
; System
; stores xt1 as the xt to be EXECd when xt2 is called
forthword_ DEFERSTORE, 0, 6, "defer!"
    ; skip past call to get to data
    twoplus_                 ; ( xt1 xt2+2 )
    call DEFGET              ; ( st1 defaddr xt2+2 )
    ; skip past data address and deferfetch section to get
    ; to deferstore section
    twoplus_                 ; ( xt1 defaddr xt2+4 )
    ; get defered store xt address
    call FETCHI              ; ( xt1 defaddr xtstore )
    jmp EXEC

; ( xt1 -- xt2 )
; System
; returns the XT associated with the given XT
forthword_ DEFERFETCH, 0, 6, "defer@"
    ; skip passed call to get to data field
    twoplus_                 ; ( xt1+2 )
    jmp DEFX

; ( i*x -- j*x )
; System
; runtime of defer
forthword_ DODEFER, 0, 7, "(defer)"
    ; top of return stack has return address of caller
    ; return address is data for defer so pop it onto data stack
    ; will not be returning to the caller
    call R_FROM         ; ( Raddr ) (R: Raddr -- )
    call DEFX           ; ( XTdefer )
    ; EXEC the defer
    jmp EXEC


; ( r-addr r-len f-addr f-len --  f) 
; String
; compares two strings in RAM

;forthword_ COMPARE, 0, 7, "compare"
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


; ( "<spaces>name" -- XT XTflags)
; Dictionary
; search dictionary for name, returns XT and its flags or throw an exception -13
forthword_ TICKF, 0, 2, "'f"
    call PNAME
    call FINDW
    dupzerosense_
    brne TICKFEXIT
    doliteral_ -13
    rjmp THROW
TICKFEXIT:
    jmp NFATOXTF

; ( "<spaces>name" -- XT )
; Dictionary
; search dictionary for name, returns XT or throw an exception -13
forthword_ TICK, 0, 1, "'"
    rcall TICKF
    drop_
    ret
    


; (addr-from addr-to n -- )
; Memory
; copy data in RAM, from lower to higher addresses

;forthword_ CMOVE, 0, 5, "cmove"
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


; ( -- addr )
; System Variable
; terminal input buffer address
forthword_ TIB, 0, 3, "tib"
    doliteral_ ram_tib
    ret

.pushsection .noinit
ram_tib: .space TIBSIZE
.popsection

; ( -- f )
; System
; refills the input buffer - flag is true if buffer has something
forthword_ REFILLTIB, 0, 10, "refill-tib"
REFILLTIB_begin:
    rcall ACCEPT               ; ( f )
    dupzerosense_              ; ( f )
    breq REFILLTIB_exit
    jmp CR
    
REFILLTIB_exit:
    ret

; ( -- addr n )
; System
; address and current length of the input buffer
forthword_ SOURCETIB, 0, 10, "source-tib"
    rcall TIB
    call SHARPTIB
    jmp FETCH

; ( -- v)
; System Value
; address of the default user area content in eeprom
forthword_ EEUSER, 0, 7, "ee-user"
  doliteral_ EE_INITUSER
  ret

; ( e-addr r-addr len -- )
; Memory
; copy len cells from eeprom to ram
forthword_ EE2RAM, 0, 6, "ee>ram"
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
forthword_ INITUSER, 0, 9, "init-user"
    rcall EEUSER
    call UP_FETCH
    doliteral_ SYSUSERSIZE
    twoslash_
    rjmp EE2RAM
    

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ STAR, 0, 1, "*"
    rcall MSTAR
    drop_
    ret

; ( u1 u2 -- u)
; Arithmetics
; multiply 2 unsigned cells to a cell
forthword_ MSTAR, 0, 2, "m*"
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
forthword_ UMSLASHMOD, 0, 6, "um/mod"
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
forthword_ NEGATE, INLINE_OPT, 6, "negate"
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
forthword_ USLASHMOD, 0, 5, "u/mod"
    movw temp2, tosl

    ld temp0, Y+
    ld temp1, Y+

    rcall DIV16_16
    rjmp SLASHMOD_end


; ( n1 n2 -- rem quot)
; Arithmetics
; signed division n1/n2 with remainder and quotient
forthword_ SLASHMOD, 0, 4, "/mod"
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

forthword_ ABS, 0, 3 "abs"
    dup_
    call LESSZERO
    ; <if>
    zerosense_
    breq ABS_exit
      jmp NEGATE
    ; <then>  
ABS_exit:
    ret


; ( -- ) 
; Tools
; stack dump
forthword_ DOTS, 0, 2, ".s"
    call SP_FETCH        ; ( limit ) setup limit
    twominus_
    rcall SP0              ; ( limit counter )
    ; <begin>
PFA_DOTS1:
    twominus_             ; ( limit counter-2 )
    call TWOOVER            ; ( limit counter-2 limit counter-2 )
    call NOTEQUAL           ; ( limit counter-2 flag )
    ; <while>
    zerosense_
    breq PFA_DOTS2
    dup_                  ; ( limit counter-2 counter-2 )
    call FETCH           ; ( limit counter-2 val )
    call UDOT            ; ( limit counter-2 )
    ; <repeat>
    rjmp PFA_DOTS1
    
PFA_DOTS2:
    jmp DDROP


; ( -- usersize ) 
; Environment
; size of the USER area in bytes
forthword_ ENVUSERSIZE, 0, 5, "/user"
    doliteral_ (SYSUSERSIZE+APPUSERSIZE)
    ret


; ( -- )
; ISR
; define the start of an Interrupt Service Routine
; must be called
forthword_ ISTART, DIS_CALL_OPT, 4, "(i:)"
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
forthword_ IEND, 0, 4, "(i;)"
    movw ZL, isrZL
    movw XL, isrXL
    movw temp0, isrt0
    movw temp2, isrt2
    
    pop isrt0
    out SREG, isrt0    
    reti
