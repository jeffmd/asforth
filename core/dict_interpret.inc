; dict_interpret.inc

; ( c-addr len wid -- [ 0 | nfa ] )
; find a word in the dictionary
; 0 if word not found in dictionary
; nfa if word is found
forthword_ FINDNFA, 0, 7, "findnfa"
    dupzerosense_
    brne FINDNFA_
      ; terminating, wid is empty
      dnip_
      ret
FINDNFA_:
    rcall FETCHE                ; ( c-addr len nfa )
    ; <begin>
FINDNFA_BEGIN:
    ; <while>
    dupzerosense_              ; ( c-addr len nfa )
    brne FINDNFA_CHECK
      ; terminating, 0 found
      dnip_                     ; ( 0 )
      ret

FINDNFA_CHECK:
    rcall TO_R                  ; ( c-addr len ) (R: nfa )
    rcall TWOOVER               ; ( c-addr len c-addr len )
    rcall R_FETCH               ; ( c-addr len c-addr len nfa )
    call ICOUNT                 ; ( c-addr len c-addr len f-addr len )
    zerohigh_                   ; ( c-addr len c-addr len f-addr 0:len )
    call ICOMPARE               ; ( c-addr len flag )
    ; <if>
    zerosense_
    breq FINDNFA_NEXT
      ; we found the string
      call DDROP               ; ( )
      call R_FROM              ; ( nfa )
      ret
    ; <then>
FINDNFA_NEXT:
      ; next try
      rcall R_FROM
      rcall NFA2LFA
      rcall FETCHI
      ; <repeat>
      rjmp FINDNFA_BEGIN


; ( addr len --  0 | nfa )
; Tools
; search root wordlists for the name from string addr/len
forthword_ FINDROOT, 0, 5, "froot"
    ; put root wid on stack
    doliteral_  pm(VE_STARTOVER) 
    ; search root
    rjmp FINDNFA_BEGIN

; ( addr len idx -- 0 | nfa )
FINDWI:
      twostar_         ; ( addr len idx*2)
      rcall CONTEXT     ; ( addr len idx*2 context )
      rcall PLUS        ; ( addr len context' )
      rcall FETCH       ; ( addr len wid )
      rjmp FINDNFA     ; ( nfa)

FINDLAST:
      drop_
      rjmp FINDROOT
      
; ( addr len --  0 | nfa )
; Tools
; search wordlists for the name from string addr/len
forthword_ FINDW, 0, 5, "findw"
    ; loop through context slots to find a word
    ; start with last slot
    rcall CONTEXT      ; ( addr len context )
    ; get context index and use as counter
    oneminus_          ; ( addr len context-1 )
    rcall CFETCH        ; ( addr len idx )
FINDW_BEGIN:
    dupzerosense_
    brmi FINDLAST      ; ( addr len idx )
      rcall TO_R        ; ( addr len ) ( R: idx )
      rcall TWOOVER     ; ( addr len addr len ) ( R: idx )
      ; get next wordlist in array
      rcall R_FROM      ; ( addr len addr len idx ) ( R: )
      dup_r_           ; ( addr len addr len idx ) ( R: idx)
      rcall FINDWI
      rcall R_FROM      ; ( addr len nfa idx ) ( R: )
      rcall SWAP        ; ( addr len idx nfa )
      dupzerosense_
      ; end loop if word found
      breq FINDW_NEXT
        dnip_          ; ( addr nfa )
        nip_           ; ( nfa )
        ret
        
FINDW_NEXT:
      drop_            ; ( addr len idx )
      ; decrement index
      oneminus_        ; ( addr len idx-1 )
      rjmp FINDW_BEGIN



; ( -- ) (R: i*x - j*x )
; System
; interpret input word by word.
forthword_ INTERPRET, 0, 9, "interpret"
    ; begin
    call PNAME     ; ( -- addr len )

    ; ?while
    dupzerosense_       ; ( -- addr len )
    breq INTERPRET_END

    rcall RECX          ; ( )
    call QSTACK        ; ( )

    ; repeat
    rjmp INTERPRET

INTERPRET_END:
    rjmp DDROP

; ( addr len -- )
; System
; recognize and execute name of word in ram using recognizer list.
forthword_ RECX, 0, 4, "recx"
    rcall ZERO            ; ( addr len n )
    rcall TO_R            ; ( addr len ) (R: n )

    ; begin
RECX_BEGIN:
    rcall R_FETCHPLUS     ; ( addr len n ) (R: n+1 )
    rcall RECFETCH       ; ( addr len recxt )

    ; ?while
    dupzerosense_        ; ( addr len recxt )
    breq RECX_DONE

    rcall TO_A            ; ( addr len ) A: recxt
    rcall TWOOVER         ; ( addr len addr len  )
    ; put addr and len on return stack so that
    ; stack can be manipulated by interpreted word
    rcall TO_R            ; ( addr len addr ) (R: n len )
    rcall TO_R            ; ( addr len ) (R: n len addr )
    rcall A_              ; ( addr len recxt ) A:
    rcall EXEC            ; ( [...] flag )
    ; restore data stack to known state
    rcall TO_A            ; ( ) A: flag
    rcall R_FROM          ; ( addr ) (R: n len )
    rcall R_FROM          ; ( addr len ) (R: n )
    rcall A_              ; ( addr len flag ) A:  

    ; <if>
    zerosense_           ; ( addr len )
    breq RECX_NEXT

    rcall DDROP         ; ( ) (R: n)
    r_drop_              ; ( ) (R: )
    ret

    ; <then>
    ; <repeat>
RECX_NEXT:
    rjmp RECX_BEGIN

RECX_DONE:
    dnip_
    drop_
    r_drop_
    ret

; ( n -- recxt )
; Interpreter
; get a recognizer from the recognizer list
; recxt is zero if at end of list
forthword_ RECFETCH, 0, 4, "rec@"
    dup_                            ; ( n n )
    doliteral_ EE_RECOGNIZERLISTLEN ; ( n n ee-reclen )
    rcall FETCHE                     ; ( n n len )
    rcall LESS                       ; ( n flag )
    ; <if>
    zerosense_                      ; ( n )
    breq PFA_RECFETCH_ZERO
      twostar_                        ; ( 2*n )
      doliteral_ EE_RECOGNIZERLIST    ; ( 2*n listaddr )
      rcall PLUS                       ; ( listaddr' )
      rjmp FETCHE                      ; ( recxt )

    ; <then>
PFA_RECFETCH_ZERO:
    zerotos_
    ret





; ( addr len -- n f )
; Interpreter
; recognizer for integer numbers
forthword_ REC_NUM, 0, 7, "rec-num"
    ; try converting to a number
    call NUMBER             ; ( [n] f )
    zerosense_
    breq PFA_REC_NONUMBER

    call STATE
    rcall FETCH              ; ( n state )
    zerosense_
    breq PFA_REC_NUMBER_OK
    call LIT
PFA_REC_NUMBER_OK:
    rjmp TRUE               ; ( n true )

PFA_REC_NONUMBER:
    rjmp ZERO               ; ( 0 )

; ( addr len -- f )
; Interpreter
; recognize a word in the dictionary
; will 
forthword_ REC_WORD, 0, 8, "rec-word"

    rcall FINDW
    dupzerosense_
    brne REC_WORD_FOUND
    ret

REC_WORD_FOUND:
    rcall NFATOXTF
    ; check if compile only word
    sbrc tosh, COMPILE_ONLY_FB 
        rjmp CHECK_IMMEDIATE
        
CHECK_COMPILE_ONLY:
    call STATE
    rcall FETCH
    zerosense_
    brne CHECK_IMMEDIATE
        ; word can only be used in compile state
        doliteral_ -14
        jmp THROW
    

CHECK_IMMEDIATE:
	; either compile or EXEC the XT
    ; check if word is immediate: bit 7 is clear if immediate
	sbrs tosh, IMMEDIATE_EN_FB ; skip next instruction if bit 7 is set: not immediate word
	    ; flag is 0: always EXEC
	    rjmp REC_WORD_EXECUTE

REC_WORD_CHECKSTATE:
        ; check state
	    call STATE
	    rcall FETCH
	    zerosense_
	    breq REC_WORD_EXECUTE
        ; in compile mode so compile xt
 	    call COMPILEXT
	    rjmp REC_WORD_OK
REC_WORD_EXECUTE:
    drop_
    ; state is zero, EXEC xt
    rcall EXEC
REC_WORD_OK:
    rjmp TRUE

; ( addr len --  )
; Interpreter
; recognizer for NOT FOUND
forthword_ REC_UNK, 0, 7, "rec-unk"
    call TYPE
    doliteral_ -13
    jmp THROW
