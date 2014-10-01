; dict_interpret.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( addr1 n1 c -- addr2 n2 )
; String
; skips leading occurances in string at addr1/n1 leaving addr2/n2 pointing to the 1st non-c character

forthword_ CSKIP, 0, 5, "cskip"
    rcall TO_B           ; ( -- addr1 n1 ) B: c
PFA_CSKIP1:
    dupzerosense_       ; ( -- addr' n')
    breq PFA_CSKIP2
    rcall OVER           ; ( -- addr' n' addr' )
    rcall CFETCH         ; ( -- addr' n' c' )
    rcall B_             ; ( -- addr' n' c' c )
    rcall EQUAL          ; ( -- addr' n' f )
    zerosense_          ; ( -- addr' n')
    breq PFA_CSKIP2
    rcall ONE
    call SLASHSTRING
    rjmp PFA_CSKIP1

PFA_CSKIP2:
    ret
    

; ( addr1 n1 c -- addr1 n2 )
; String
; Scan string at addr1/n1 for the first occurance of c, leaving addr1 n2,
; char at n2 is first non-c character

forthword_ CSCAN, 0, 5, "cscan"
    niptob_           ; ( -- addr1 c ) B: n1
    rcall OVER         ; ( -- addr1 c addr1 )
    copytoa_          ; ( -- addr1 c addr1 ) A: addr1
    zerotos_          ; ( -- addr1 c 0 )
PFA_CSCAN1:
    dup_              ; ( -- addr1 c n2 n2 )
    rcall B_           ; ( -- addr1 c n2 n2 n1 )
    rcall LESS         ; ( -- addr1 c n2 flag )
    zerosense_        ; ( -- addr1 c n2 )
    breq PFA_CSCAN3
    rcall OVER         ; ( -- addr1 c n2 c )
    rcall ACFETCH      ; ( -- addr1 c n2 c c')
    aplus_
    rcall EQUAL        ; ( -- addr1 c n2 f)
    zerosense_        ; ( -- addr1 c n2 )
    breq PFA_CSCAN2
    nip_              ; ( -- addr1 n2 )
    ret

PFA_CSCAN2:
    oneplus_          ; ( -- addr1 c n2+1 )
    rjmp PFA_CSCAN1   ; ( -- addr1 c n2+1 )

PFA_CSCAN3:
    rcall DDROP      ; ( -- addr1)
    jmp B_            ; ( -- addr1 n1)



; ( r-addr r-len f-addr f-len --  f)
; Memory
; compares string in RAM with string in flash
forthword_ ICOMPARE, 0, 8, "icompare"
    rcall TO_B               ; ( r-addr r-len f-addr)  B: f-len
    rcall OVER               ; ( r-addr r-len f-addr r-len)
    rcall B_                 ; ( r-addr r-len f-addr r-len f-len )
    ; check if strings are same length
    rcall NOTEQUAL           ; ( r-addr r-len f-addr flag )
    ; if
    zerosense_              ; ( r-addr r-len f-addr )
    breq PFA_ICOMPARE_SAMELEN

    ; strings are not the same length
    dnip_
    zerotos_                ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_SAMELEN:
    twostar_                ; ( r-addr len f-addr*2 )
    rcall TO_A               ; ( r-addr len ) A: f-addr
    oneplus_                ; ( r-addr len+1 )
    twoslash_               ; ( r-addr (len+1)/2 )

    ; begin
PFA_ICOMPARE_LOOP:
    copytob_                ; ( r-addr len ) B: len
    ; while
    zerosense_              ; ( r-addr )
    breq PFA_ICOMPARE_DONE

    dup_                    ; ( r-addr r-addr )
    rcall FETCH              ; ( r-addr r-cc )
    rcall AFETCHI            ; ( r-addr r-cc f-cc )
    aplus_
    ; flash strings are zero-padded at the last cell
    ; that means: if the flash cell is less $0100, than mask the
    ; high byte in the ram cell
    dup_                    ; ( r-addr r-cc f-cc f-cc )
    doliteral_ 0x100        ; ( r-addr r-cc f-cc f-cc 0x100 )
    rcall ULESS               ; ( r-addr r-cc f-cc flag)
    ; if
    zerosense_
    breq PFA_ICOMPARE_LASTCELL
    rcall SWAP               ; ( r-addr f-cc r-cc)
    zerohigh_               ; ( r-addr f-cc 0:r-cc)
    ; then
PFA_ICOMPARE_LASTCELL:
    rcall NOTEQUAL           ; ( r-addr flag)
    ; if
    zerosense_              ; ( r-addr )
    breq PFA_ICOMPARE_NEXTLOOP

    ; strings don't match
    zerotos_                ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_NEXTLOOP:
    twoplus_              ; ( r-addr+2 )
    rcall B_               ; ( r-addr+2 len )
    oneminus_             ; ( r-addr+2 len-1 )

    ; repeat
    rjmp PFA_ICOMPARE_LOOP

PFA_ICOMPARE_DONE:
    ; strings are the same
    drop_
    doliteral_ -1
    ret
    
; ( -- addr n )
; System
; address and current length of the input buffer
forthword_ SOURCE, 0, 6, "source"
    call DODEFER
    .word USER_SOURCE
    addr_ FETCHU
    addr_ STOREU

; ( addr1 u1 n -- addr2 u2 )
; String
; adjust string from addr1 to addr1+n, reduce length from u1 to u2 by n
forthword_ SLASHSTRING, 0, 2, "/$"
    rcall OVER    ; ( -- addr1 u1 n u1)
    call MIN     ; ( -- addr1 u1 n|u1)
    rcall ROT     ; ( -- u1 n addr1 )
    rcall OVER    ; ( -- u1 n addr1 n)
    rcall PLUS    ; ( -- u1 n addr2 )
    rcall RROT    ; ( -- addr2 u1 n )
    rjmp MINUS   ; ( -- addr2 u2)

; ( -- srcaddr len  )
; String
; Adjust the source addr using >in.
forthword_ SRCIN, 0, 5, "srcin"
    rcall SOURCE         ; ( srcaddr len )
    rcall G_IN           ; ( srcaddr len ginaddr )
    rcall FETCH           ; ( srcaddr len gin)
    rjmp SLASHSTRING     ; ( srcaddr' len' )

; ( -- )
; Input
; skip space in input source.
forthword_ SKIPBL, 0, 6, "skipbl"
    rcall SRCIN          ; ( srcaddr len )
    call BL             ; ( srcaddr' len' c )
    rcall CSKIP          ; ( addr2 n2 )

    ; adjust >IN
    drop_                ; ( addr2 )
    rcall SOURCE         ; ( addr2 srcaddr len )
    drop_                ; ( addr2 srcaddr )
    rcall MINUS           ; ( gpos )
    rcall G_IN           ; ( gpos ginaddr )
    rjmp STORE            ; (  )


; ( char "ccc<char>" -- c-addr u )
; String
; in input buffer parse ccc delimited string by the delimiter char.
forthword_ PARSE, 0, 5, "parse"
    rcall TO_R        ; ( -- ) (R: c )
    rcall SRCIN      ; ( -- addr len)
    rcall R_FROM      ; ( -- addr' len' c)
    rcall CSCAN      ; ( -- addr' len'')
    dup_             ; ( -- addr' len'' len'')
    oneplus_         ; ( -- addr' len'' len''+1 )
    rcall G_IN       ; ( -- addr' len'' len''+1 >in)
    rjmp PLUSSTORE    ; ( -- addr' len'')


; ( "<name>" -- c-addr len )
; String
; In the SOURCE buffer parse whitespace delimited string. Returns string address within SOURCE.
forthword_ PNAME, 0, 5, "pname"
    
    rcall SKIPBL
    call BL
    rjmp PARSE
            
    

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
    
; ( -- )
; System
; check if a ready prompt is required
forthword_ QP_RD, 0, 5, "?p_rd"
    rcall STATE
    rcall FETCH
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
forthword_ DOTASKS, 0, 7, "dotasks"
    ; <begin>
      rcall DOREFILL
      rcall PAUSE
    ; <again>
    rjmp DOTASKS

; ( -- )
; System
; reset stacks and start over again
forthword_ QUIT, 0, 4, "quit"
    ; reset data stack
    call SP0
    rcall SP_STORE
    ; reset return stack pointers
    call RP0
    rcall RP_STORE
    ; switch to interpret mode
    rcall LBRACKET
    
    rjmp DOTASKS
    
; ( -- )
; System
; wait for the input buffer to be refilled then interpret
forthword_ DOREFILL, 0, 8, "dorefill"
  ; if just starting new input line then check prompt and reset input position
  rcall G_IN               ; ( >inaddr )
  rcall FETCH              ; ( >in )
  zerosense_              ; ( )
  breq DOREFILL_do
    rcall ZERO             ; ( 0 )
    dup_                  ; ( 0 0 )
    rcall G_IN             ; ( 0 0 >inaddr )
    rcall STORE            ; ( 0 )
    ; reset input buffer cursor position
    call SHARPTIB           ; ( 0 #tibaddr )
    rcall STORE               ; ( )
    rjmp QP_RD
    
DOREFILL_do:
  ; refill the input buffer
  call REFILL             ; ( f )
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
forthword_ DOINTERPRET, 0, 11, "dointerpret"
      ; input buffer has something so try to interpret it
      ; setup exception handling in case of error
  doliteral_ pm(INTERPRET)
  call CATCH
  ; check to make sure data stack is ok
  rcall QDUP
  ; <if>
  zerosense_
  breq DOINTERPRET_ok
    dup_
	doliteral_ -2
	rcall LESS
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
  ;call ONE                ; ( 1 )
  rcall G_IN               ; ( 1 >inaddr )
  rcall ONEPLUSSTORE       ; ( )
  rjmp PROMPTOK

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTRDY, 0, 4, "p_rd"
    call CR
    call DOSLITERAL
    ascii_ 2, "> "
    jmp ITYPE

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTCOMPILE, 0, 4, "p_cp"
    call CR
    call DOSLITERAL
    ascii_ 2, ": "
    jmp ITYPE

; ( -- )
; System
; send the OK prompt to the command line
forthword_ PROMPTOK, 0, 4, "p_ok"
    call DOSLITERAL
    ascii_ 3, " ok"
    jmp ITYPE

; ( n -- )
; System
; process the error prompt
forthword_ PROMPTERROR, 0, 4, "p_er"
	call DOSLITERAL
	ascii_ 4, " ?? "
    call ITYPE
    ; save base to return stack
	call BASE
	rcall FETCH
	rcall TO_R
	; make sure in decimal mode
	call DECIMAL
	call DOT
	; print the offending word
	rcall G_IN
	rcall FETCH
	call DOT
	; restore base
	rcall R_FROM
	jmp BASESTORE

; ( -- )
; Multitasking
; Fetch pause vector and EXEC it. may make a context/task switch
forthword_ PAUSE, 0, 5, "pause"
    call DODEFER
    .word ram_pause
    addr_ FETCH
    addr_ STORE

.pushsection .noinit
ram_pause: .space 2
.popsection
