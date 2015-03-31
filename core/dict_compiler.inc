; dict_compiler.inc - basic compiler words


; ( -- addr )
; System Variable
; system state variable. 0 is interpret mode. Not 0 is compile mode
forthword_ STATE, 0, "state"
    call DOUSER
    .word USER_STATE
    ret

forthword_ STATEFETCH, 0, "state@"
    rcall STATE
    rjmp FETCH

.pushsection .noinit
ram_CURRENT: .space 2
.popsection

; ( -- addr ) 
; System Variable
; current vocabulary for new words 
forthword_ CURRENT, 0, "current"
    doliteral_ ram_CURRENT
    ret

.pushsection .noinit
ram_CONTEXTidx: .space 1
ram_CONTEXT: .space 2 * 5 ; 5 wordlists max
.popsection

; ( -- addr ) 
; System Variable
; current vocabulary for searching
; array of wordlists
forthword_ CONTEXT, 0, "context"
    doliteral_ ram_CONTEXT
    ret

.pushsection .noinit
ram_LATEST: .space 2
.popsection

; ( -- addr ) 
; System Variable
; system LATEST. Get flash program address of latest word being compiled. 
forthword_ LATEST, 0, "latest"
    doliteral_ ram_LATEST
    ret

.pushsection .noinit
COLON_SMUDGE: .space 2
.popsection

; ( -- addr ) 
; System Variable
; system SMUDGE. Get flash program address of latest word being compiled. 
forthword_ SMUDGE, 0, "smudge"

    doliteral_ COLON_SMUDGE
    ret


.pushsection .noinit
ram_dp: .space 2
.popsection


; ( -- f-addr )
; System Value
; address of the next free dictionary cell
forthword_ DP, 0, "dp"
  doliteral_ ram_dp
  rjmp FETCH

; ( -- )
; Compiler
; backup DP to eeprom
forthword_ DPSTOREE, 0, "dp!e"
  rcall DP
  doliteral_ EE_DP
  rjmp STOREE 

; ( addr -- )
; System Value
; store address of the next free dictionary cell
forthword_ DPSTORE, 0, "dp!"
  doliteral_ ram_dp
  rjmp STORE

; ( -- )
; Compiler
; fetch DP from eeprom and put in ram
forthword_ DPFETCHE, 0, "dp@e"
  doliteral_ EE_DP
  rcall FETCHE 
  rjmp DPSTORE

; ( offset -- )
; Dictionary
; add offset to 16 bit (dictionary pointer) DP
forthword_ DPPLUS, 0, "dp+"
  doliteral_ ram_dp
  rjmp PLUSSTORE

; ( -- )
; Dictionary
; increment 16 bit (dictionary pointer) DP by one
forthword_ DPPLUSONE, 0, "dp+1"
    rcall ONE
    rjmp DPPLUS



; ( -- )
; tail call optimize: change last rcall/call into rjmp/jmp
forthword_ TAILOPT, 0, ";opt"
    ; optimize only if smudge is not zero
    rcall SMUDGE
    rcall FETCH
    zerosense_
    brne TAILOPT_OPT
    ret
    
TAILOPT_OPT:
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
    rcall DP       ; ( DP )
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
    rcall DP            ; ( 0x940c DP )
    twominus_          ; ( 0x940c DP-2 )
    rjmp STOREI        ; (  )

; ( -- )
; Compiler
; update wid from smudge if valid 
forthword_ UWID, 0, "uwid"
    rcall SMUDGE
    rcall FETCH
    rcall QDUP
    zerosense_
    breq UWID_end
       rcall CURRENT
       rcall FETCH
       rcall STOREE
       rcall SMUDGE
       rcall ZEROSTORE
       rjmp DPSTOREE
UWID_end:
    ret

; ( --  ) 
; Compiler
; reset forth flags
forthword_ FFRST, 0, "ffrst"
    ; clear rcall/call flags
    cbr fflags, (1<<LAST_RCALL_FB)|(1<<LAST_CALL_FB)
    ; set disable tail optimize flag
    sbr fflags, 1<<DIS_CALL_OPT_FB
    ret
    
; ( --  ) 
; Compiler
; enter interpreter mode
forthword_ LBRACKET, IMMEDIATE_EN, "["
    rcall STATE
    rjmp ZEROSTORE


; ( n -- )
; Dictionary
; compile 16 bit into flash at (dictionary pointer) DP
forthword_ COMMA, 0, ","
    rcall DP
    rcall STOREI
    rjmp DPPLUSONE

; ( addr len len' -- )
; Compiler
; compiles a string from RAM to Flash
forthword_ DOSCOMMA, 0, "(s,)"
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
      rjmp COMMA      ; ( )

    ; then
PFA_SCOMMA3:
    ret
   
; ( start match -- start count )
; match is the word value to search for in the flash
; start is the start address of the search in flash
; the search goes up in flash address space
; count is number of words to get to match value in flash
; or get word index of match
forthword_ FINDI, 0, "findi"
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
forthword_ INLINE, 0, "inline"
    ; set disable tail call optimize flag:
    sbr fflags, 1<<DIS_CALL_OPT_FB
    ; find the ret word, want count to ret
    ; count will be number of words to inline
    doliteral_ 0x9508        ; ( start 0x9508 )
    rcall FINDI              ; ( start count )

    ; dp swap dup dp+ itoi
    rcall DP                  ; ( start count dp )
    rcall SWAP               ; ( start dp count )
    dup_                     ; ( start dp count count )
    rcall DPPLUS             ; ( start dp count )
    rjmp ITOI


; ( nfa -- lfa )
; System
; get the link field address from the name field address
forthword_ NFA2LFA, 0, "nfa>lfa"
    rcall ICOUNT 
    zerohigh_
    rcall WALIGN
    rjmp PLUS
    
; ( nfa -- [ 0 ] | [ xt xtflags] )
; convert name field address to xt and xtflags
forthword_ NFATOXTF, 0, "nfa>xtf"
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
    
    
; ( -- ) 
; Dictionary
; set the context and current to root forth vocabulary and remove
; other vocabularies
forthword_ ONLY, IMMEDIATE_EN, "only"
    ; forth vocab always takes slot 0
    doliteral_ EE_FORTHWORDLIST
    dup_
    rcall CONTEXT
    rcall STORE
    ; make current also forth vocab
    rcall CURRENT
    rcall STORE
    ; move index to next slot
    rcall ONE
    rcall CONTEXT
    oneminus_
    rcall CSTORE
    ; make second slot have no vocab
    rcall CONTEXT
    twoplus_
    rjmp ZEROSTORE
