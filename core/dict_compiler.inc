
; ( -- addr ) 
; System Variable
; system LATEST. Get flash program address of latest word being compiled. 
forthword_ LATEST, 0x8006, "latest"
    doliteral_ ram_LATEST
    ret

.pushsection .data
ram_LATEST: .space 2
.popsection


; ( -- ) (C: "<spaces>name" -- )
; Compiler
; create a named entry in the dictionary, XT is DO_COLON

forthword_ COLON, 0x8001, ":"
    call DOCREATE
    doliteral_ COLON_SMUDGE
    rcall STORE
    call DP
    rcall LATEST
    rcall STORE
    rjmp RBRACKET

.pushsection .data
COLON_SMUDGE: .space 2
.popsection

; ( -- )
; tail call optimize: change last rcall/call into rjmp/jmp
;forthword_ TAILOPT, 0x8007, "tailopt"
TAILOPT:
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
; finish colon defintion, compiles (exit) and returns to interpret state 
forthword_ SEMICOLON, 0x0001, ";"
    ; check fflags to see if tail call optimization can be done
    sbrs fflags, DIS_CALL_OPT_FB
    rcall TAILOPT
    sbrc fflags, DIS_CALL_OPT_FB
    ; don't optimize tail call, just compile a ret.
    call RET_C          ; ( )

    rcall LBRACKET
    doliteral_ COLON_SMUDGE
    rcall FETCH
    rcall QDUP
    zerosense_
    breq SEMICOLON_end
       rcall GET_CURRENT
       rcall STOREE
       rcall ZERO
       doliteral_ COLON_SMUDGE
       rjmp STORE
SEMICOLON_end:
    ret

; ( --  ) 
; Compiler
; enter compiler mode
forthword_ RBRACKET, 0x8001, "]"
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
forthword_ LBRACKET, 0x0001, "["
    rcall ZERO
    call STATE
    rjmp STORE


; ( -- c ) (C: "<space>name" -- )
; Tools
; skip leading space delimites, place the first character of the word on the stack

forthword_ BRACKETCHAR, 0x0006, "[char]"
    call CHAR
    jmp LIT

; ( i*x x1 -- | i*x ) (R: j*y -- | j*y ) (C: "ccc<quote>" -- )
; Exceptions
; check flag. If true display the parsed text and throw exception -2 

forthword_ ABORTSTRING, 0x0006, "abort\""
    ; postpone if
    call DOCOMPILE
    addr_ 0 ; ******* FIXME
    call DP
    call DOCOMPILE
    .word -1
    
    ;rcall DOTSTRING
    
    call DOCOMPILE
    addr_ -1 ; DOLITERAL ******* FIXME
    
    call DOCOMPILE
    .word -2
    call DOCOMPILE
    addr_ THROW
    ; then
    call DP
    rcall SWAP
    rjmp STOREI

; ( i*x -- ) (R: j*y -- )
; Exceptions
; send an exception -1

forthword_ ABORT, 0x8005, "abort"
    doliteral_ -1
    jmp THROW



; ( -- faddr len ) 
; Environment
; flash address of the amforth name string
forthword_ ENV_FORTHNAME, 0x8005, "fname"
    call DOSLITERAL
    ascii_ 7, "asforth" ; a simplified forth based on amforth
    ret

; ( -- n ) 
; Environment
; version number of amforth
forthword_ ENV_FORTHVERSION, 0x8007, "version"
    doliteral_ 11
    ret

; ( -- faddr len ) 
; Environment
; flash address of the CPU identification string
forthword_ ENV_CPU, 0x8003, "cpu"
    doliteral_ pm(mcu_name)
    jmp ICOUNT

; ( -- faddr len ) 
; Environment
; flash address of some CPU specific parameters
forthword_ ENV_MCUINFO, 0x8008, "mcu-info"
    doliteral_ pm(mcu_info)
    ret

; ( -- usersize ) 
; Environment
; size of the USER area in bytes
forthword_ ENVUSERSIZE, 0x8005, "/user"
    doliteral_ (SYSUSERSIZE+APPUSERSIZE)
    ret

; ( ee-addr -- itemn .. item0 n) 
; Tools
; Get an array from EEPROM and put on the stack
forthword_ FETCH_EE_ARRAY, 0x8004, "@e[]"
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
    call TWODROP       ; ( ... item_i )
    rjmp A_            ; ( ... item_i len )

; ( recn .. rec0 n ee-addr -- ) 
; Tools
; Write a list to EEPROM
forthword_ STORE_EE_ARRAY, 0x8004, "!e[]"
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

forthword_ COMPARE, 0x8007, "compare"
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
forthword_ NFA2LFA, 0x8007, "nfa>lfa"
    call ICOUNT 
    zerohigh_
    rcall WALIGN
    rjmp PLUS

; ( -- wid) 
; Dictionary
; get the wid of the current compilation word list
forthword_ GET_CURRENT, 0x8003, "wid"
    doliteral_ EE_FORTHWORDLIST
    ret