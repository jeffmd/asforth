
; ( -- addr ) 
; System Variable
; system LATEST. Get flash program address of latest word being compiled. 
forthword_ LATEST, 0, 6, "latest"
    doliteral_ ram_LATEST
    ret

.pushsection .data
ram_LATEST: .space 2
.popsection


; ( -- addr ) 
; System Variable
; system SMUDGE. Get flash program address of latest word being compiled. 
forthword_ SMUDGE, 0, 6, "smudge"

    doliteral_ COLON_SMUDGE
    ret

.pushsection .data
COLON_SMUDGE: .space 2
.popsection

; ( -- ) (C: "<spaces>name" -- )
; Compiler
; create a named entry in the dictionary, XT is DO_COLON
forthword_ COLON, 0, 1, ":"

    rcall DOCREATE
    rcall SMUDGE
    rcall STORE
    call DP
    rcall LATEST
    rcall STORE
    rjmp RBRACKET

; ( -- )
; tail call optimize: change last rcall/call into rjmp/jmp
forthword_ TAILOPT, IMMEDIATE_EN, 4, ";opt"
    ; check fflags to see if tail call optimization can be done
    sbrc fflags, DIS_CALL_OPT_FB
    ; don't optimize tail call, just compile a ret.
    jmp RET_C          ; ( )
    
    ; bit 0 and 1 of fflags indicate type of call and number of words to go back in DP
    ; if bit 0 set then its a rcall that has to turned into a rjmp
    sbrs fflags, LAST_RCALL_FB
    rjmp TAILOPT_CALL

    ;   rcall           ->       rjmp
    ; 1101kkkk kkkkkkkk -> 1100kkkk kkkkkkkk
    ; fetch contents at DP-1
    call DP       ; ( DP )
    oneminus_      ; ( DP-1 )
    dup_           ; ( DP-1 DP-1 )
    rcall FETCHI   ; ( DP-1 inst )
    ; bit 4 of high byte must be cleared.
    cbr tosh, 1<<4    ; ( DP-1 inst' )
    ; store result at DP-1
    rcall SWAP     ; ( inst' DP-1 )
    rjmp STOREI     ; (  )
    
TAILOPT_CALL:
    
    ; if bit 1 set then its a call that has to be turned into a jmp
    sbrs fflags, LAST_CALL_FB
    ret

    ;          call                      ->             jmp
    ; 1001010k kkk111k kkkkkkkk kkkkkkkk -> 1001010k kkk110k kkkkkkkk kkkkkkkk
    ; assume <64k address space so can directly replace first part of instruction, don't need to read it in
    ; bit 1 of high word low byte must be cleared.
    ; write in 0x940C at DP-2
    doliteral_ 0x940C  ; ( 0x940c )
    call DP            ; ( 0x940c DP )
    twominus_          ; ( 0x940c DP-2 )
    rjmp STOREI        ; (  )



; ( -- )
; Compiler
; update wid from smudge if valid 
forthword_ UWID, IMMEDIATE_EN, 4, "uwid"
    doliteral_ COLON_SMUDGE
    rcall FETCH
    rcall QDUP
    zerosense_
    breq UWID_end
       rcall GET_CURRENT
       rcall STOREE
       rcall ZERO
       doliteral_ COLON_SMUDGE
       rcall STORE
       jmp DPSTOREE
UWID_end:
    ret


; ( -- )
; Compiler
; finish colon defintion, compiles (exit) and returns to interpret state 
forthword_ SEMICOLON, IMMEDIATE_EN, 1, ";"
    rcall TAILOPT
    rcall LBRACKET
    rjmp UWID
    
; ( --  ) 
; Compiler
; enter compiler mode
forthword_ RBRACKET, 0, 1, "]"
    ; clear rcall/call flags
    cbr fflags, (1<<LAST_RCALL_FB)|(1<<LAST_CALL_FB)
    ; set disable tail optimize flag
    sbr fflags, 1<<DIS_CALL_OPT_FB
    rcall ONE
    call STATE
    rjmp STORE

; ( --  ) 
; Compiler
; enter interpreter mode
forthword_ LBRACKET, IMMEDIATE_EN, 1, "["
    rcall ZERO
    call STATE
    rjmp STORE


; ( -- c ) (C: "<space>name" -- )
; Tools
; skip leading space delimites, place the first character of the word on the stack

forthword_ BRACKETCHAR, IMMEDIATE_EN, 6, "[char]"
    call CHAR
    jmp LIT

; ( addr len len' -- )
; Compiler
; compiles a string from RAM to Flash
forthword_ DOSCOMMA, 0, 4, "(s,)"
    rcall COMMA        ; ( addr len )
    rcall BTOW         ; ( addr len/2 rem )
    rcall TO_R         ; ( addr len/2 ) (R: rem )
    niptoa_            ; ( len/2 ) A: addr

    ; begin
PFA_SCOMMA1:
    ; ?while
    dupzerosense_      ; ( len )
    breq PFA_SCOMMA2
    rcall AFETCH       ; ( len c1c2 )
    aplus_
    rcall COMMA        ; ( len )
    oneminus_          ; ( len-1 )
    ; repeat
    rjmp PFA_SCOMMA1

PFA_SCOMMA2:
    drop_              ; (  )
    rcall R_FROM       ; ( rem ) (R: )
    ; if
    zerosense_         ; ( )
    breq PFA_SCOMMA3
      rcall ACFETCH    ; ( c1 )
      rcall COMMA      ; ( )

    ; then
PFA_SCOMMA3:
    ret

; ( addr len -- )
; Compiler
; compiles a string from RAM to Flash
forthword_ SCOMMA, 0, 2, "s,"
    dup_
    ; fall thru into DOSCOMMA
    rjmp DOSCOMMA

; ( n -- )
; Dictionary
; compile 16 bit into flash at (dictionary pointer) DP
forthword_ COMMA, 0, 1, ","
    call DP
    rcall STOREI
    rjmp DPPLUSONE

; ( -- )
; Dictionary
; increment 16 bit (dictionary pointer) DP by one
forthword_ DPPLUSONE, 0, 4, "dp+1"
    rcall ONE
    rjmp DPPLUS

; ( offset -- )
; Dictionary
; add offset to 16 bit (dictionary pointer) DP
forthword_ DPPLUS, 0, 3, "dp+"
    call DP
    rcall PLUS
    call DOTO
    addr_ DP
    ret

; ( --  ) (C: "<spaces>name" -- voc-link )
; Compiler
; parse the input and create an vocabulary entry without XT and data field (PF)
forthword_ DOCREATE, 0, 8, "(create)"
    call PNAME
    rcall GET_CURRENT
    ; fall thru into HEADER
    rjmp HEADER

; ( addr len wid -- voc-link )
; Compiler
; creates the vocabulary header without XT and data field (PF) in the wordlist wid
forthword_ HEADER, 0, 6, "header"
    call DP              ; ( addr len wid DP )
    rcall TO_R           ; ( addr len wid ) (R: DP )
    rcall TO_R           ; ( addr len ) (R: DP wid )
    ; <if>
    dupzerosense_
    breq HEADER_error
      dup_                ; ( addr len len )
      ldi tosh, 0xFF      ; ( addr len len' )
      rcall DOSCOMMA      ; ( )
      ; make voc link
      rcall R_FROM        ; ( wid ) (R: DP )
      call FETCHE         ; ( VE_HEAD )
      rcall COMMA         ; ( )
      rcall R_FROM         ; ( DP ) (R: )
      ret

    ; <then>
HEADER_error:
    ; -16: attempted to use zero length string as a name
    doliteral_ -16
    jmp THROW


; ( -- )
; Dictionary
; read the following cell from the dictionary and append it to the current dictionary position.
; must use call/rcall

forthword_ DOCOMPILE, DIS_CALL_OPT, 9, "(compile)"
    rcall R_FETCHPLUS    ; ( raddr ) (R: raddr+1)
    rcall FETCHI         ; ( nfa )
    rcall NFATOXTF       ; ( xt xtflags )
    jmp COMPILEXT
    
    
; ( start match -- start count )
; match is the word value to search for in the flash
; start is the start address of the search in flash
; the search goes up in flash address space
; count is number of words to get to match value in flash
; or get word index of match
forthword_ FINDI, 0, 5, "findi"
    ; fetch from flash and check if equal to match word
    rcall TO_B       ; ( start ) B: match
    dup_             ; ( start start )
    twostar_         ; ( start start*2 )
    rcall TO_A       ; ( start ) A: start*2
    ; begin
FINDI_BEGIN:
      rcall AFETCHI  ; ( start val )
      aplus_
      pushtos
      movw tosl, bl  ; ( start val match )
      rcall EQUAL    ; ( start flag )
    ; until
    zerosense_       ; ( start )
    breq FINDI_BEGIN

    rcall A_         ; ( start end )
    twoslash_        ; ( start end/2 )
    oneminus_        ; ( start end/2-1 )
    rcall OVER       ; ( start end/2-1 start )
    rjmp MINUS       ; ( start count )


; ( start -- )
; compiler
; copy code to current dictionary entry
forthword_ INLINE, 0, 6, "inline"
    ; set disable tail call optimize flag:
    sbr fflags, 1<<DIS_CALL_OPT_FB
    ; find the ret word, want count to ret
    ; count will be number of words to inline
    doliteral_ 0x9508        ; ( start 0x9508 )
    rcall FINDI              ; ( start count )

    ; dp swap dup dp+ itoi
    call DP                  ; ( start count dp )
    rcall SWAP               ; ( start dp count )
    dup_                     ; ( start dp count count )
    rcall DPPLUS             ; ( start dp count )
    jmp ITOI



; ( i*x x1 -- | i*x ) (R: j*y -- | j*y ) (C: "ccc<quote>" -- )
; Exceptions
; check flag. If true display the parsed text and throw exception -2 

;forthword_ ABORTSTRING, DIS_CALL_OPT, 6, "abort\""
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

; ( i*x -- ) (R: j*y -- )
; Exceptions
; send an exception -1

forthword_ ABORT, 0, 5, "abort"
    doliteral_ -1
    jmp THROW



; ( -- faddr len ) 
; Environment
; flash address of the amforth name string
forthword_ ENV_FORTHNAME, 0, 5, "fname"
    call DOSLITERAL
    ascii_ 7, "asforth" ; a simplified forth based on amforth
    ret

; ( -- n ) 
; Environment
; version number of amforth
forthword_ ENV_FORTHVERSION, 0, 7, "version"
    doliteral_ 11
    ret

; ( -- faddr len ) 
; Environment
; flash address of the CPU identification string
forthword_ ENV_CPU, 0, 3, "cpu"
    doliteral_ pm(mcu_name)
    jmp ICOUNT

; ( -- faddr len ) 
; Environment
; flash address of some CPU specific parameters
forthword_ ENV_MCUINFO, 0, 8, "mcu-info"
    doliteral_ pm(mcu_info)
    ret

; ( -- usersize ) 
; Environment
; size of the USER area in bytes
forthword_ ENVUSERSIZE, 0, 5, "/user"
    doliteral_ (SYSUSERSIZE+APPUSERSIZE)
    ret

; ( ee-addr -- itemn .. item0 n) 
; Tools
; Get an array from EEPROM and put on the stack
forthword_ FETCH_EE_ARRAY, 0, 4, "@e[]"
    dup_               ; ( ee-addr ee-addr )
    rcall FETCHE       ; ( ee-addr len )
    rcall _A           ; ( ee-addr len ) A: len
    twostar_           ; ( ee-addr blen )
    ; go from bigger to smaller addresses
PFA_FETCH_EE_ARRAY1:
    copytob_           ; ( ee-addr2 blen )
    dupzerosense_      ; ( ee-addr2 blen)
    breq PFA_FETCH_EE_ARRAY2
    rcall OVER         ; ( ee-addr2 blen ee-addr2 )
    rcall PLUS          ; ( ee-addr2 ee-addr3 )
    rcall FETCHE       ; ( ee-addr2 item_i )
    rcall SWAP         ; ( item_i ee-addr2 )
    rcall B_           ; ( item_i ee-addr2-2 blen)
    twominus_          ; ( item_i ee-addr2-2 blen-2)
    rjmp PFA_FETCH_EE_ARRAY1
PFA_FETCH_EE_ARRAY2:
    call DDROP       ; ( ... item_i )
    rjmp A_            ; ( ... item_i len )

; ( recn .. rec0 n ee-addr -- ) 
; Tools
; Write a list to EEPROM
forthword_ STORE_EE_ARRAY, 0, 4, "!e[]"
    rcall TWOOVER   ; ( recn .. rec0 n ee-addr n ee-addr )
    rcall STOREE ; ( recn .. rec0 n e-addr )
    rcall SWAP   ; ( recn .. rec0 e-addr n )
PFA_STORE_EE_ARRAY1:
    copytob_       ; ( recn .. rec0 e-addr n ) B: n
    zerosense_
    breq PFA_STORE_EE_ARRAY2
    twoplus_       ; ( i_x e-addr+2 )
    rcall TUCK     ; ( e-addr+2 i_x e-addr+2 )
    rcall STOREE   ; ( e-addr+2 )
    rcall B_       ; ( e-addr+2 n )
    oneminus_      ; ( e-addr+2 n-1 )
    rjmp PFA_STORE_EE_ARRAY1
PFA_STORE_EE_ARRAY2:
    drop_
    ret


; ( r-addr r-len f-addr f-len --  f) 
; String
; compares two strings in RAM

forthword_ COMPARE, 0, 7, "compare"
    movw temp0, tosl
    poptos
    movw XL, tosl
    poptos
    movw temp2, tosl
    poptos
    movw ZL, tosl
PFA_COMPARE_LOOP:
    ld temp4, X+
    ld temp5, Z+
    cp temp4, temp5
    brne PFA_COMPARE_NOTEQUAL
    dec temp0
    breq PFA_COMPARE_ENDREACHED2
    dec temp2
    brne PFA_COMPARE_LOOP
    rjmp PFA_COMPARE_ENDREACHED
PFA_COMPARE_ENDREACHED2:
    dec temp2
PFA_COMPARE_ENDREACHED:
    or temp0, temp2
    brne PFA_COMPARE_CHECKLASTCHAR
    clr tosl
    rjmp PFA_COMPARE_DONE
PFA_COMPARE_CHECKLASTCHAR:
PFA_COMPARE_NOTEQUAL:
    ser tosl

PFA_COMPARE_DONE:
    mov tosh, tosl
    ret

; ( nfa -- lfa )
; System
; get the link field address from the name field address
forthword_ NFA2LFA, 0, 7, "nfa>lfa"
    call ICOUNT 
    zerohigh_
    rcall WALIGN
    rjmp PLUS
    
; ( nfa -- [ 0 ] | [ xt xtflags] )
; convert name field address to xt and xtflags
forthword_ NFATOXTF, 0, 7, "nfa>xtf"
    dupzerosense_              ; ( n )
    breq NFATOXTF_EXIT

    dup_                        ; ( nfa nfa )
    rcall FETCHI                ; ( nfa xtflags )
    rcall TO_R                  ; ( nfa ) (R: xtflags)
    rcall NFA2LFA               ; ( lfa )
    oneplus_   ; lfa>xt         ; ( xt )
    rcall R_FROM                ; ( xt xtflags )

NFATOXTF_EXIT:
    ret

    

; ( -- wid) 
; Dictionary
; get the wid of the current compilation word list
forthword_ GET_CURRENT, 0, 3, "wid"
    doliteral_ EE_FORTHWORDLIST
    ret
