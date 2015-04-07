; dict_interpret.inc
; forth words that make up the forth interpreter


.pushsection .noinit
ram_tib: .space TIBSIZE
.popsection

; ( -- addr )
; System Variable
; terminal input buffer address
forthword_ TIB, 0, "tib"
    doliteral_ ram_tib
    ret

.pushsection .noinit
ram_sharptib: .space 2
.popsection

; ( -- addr )
; variable holding the number of characters in TIB
forthword_ SHARPTIB, 0, "#tib"
    doliteral_ ram_sharptib
    ret

; ( -- a-addr )
; pointer to current read position in input buffer
forthword_ G_IN, 0, ">in"
    call DOUSER
    .word USER_G_IN

; ( -- addr len )
; System
; address and current length of the input buffer
forthword_ STIB, 0, "stib"
    rcall TIB
    rcall SHARPTIB
    rjmp FETCH

; ( c -- )
; fetch the emit vector and EXEC it. should emit a character from TOS
forthword_ EMIT, 0, "emit"
    call DODEFER
    .word USER_EMIT
    addr_ FETCHU
    addr_ STOREU

; ( -- c )
; fetch key vector and EXEC it, should leave a single character on TOS
forthword_ KEY, 0, "key"
    call DODEFER
    .word USER_KEY
    addr_ FETCHU
    addr_ STOREU

; ( -- )
; cause subsequent output appear at the beginning of the next line
forthword_ CR, 0, "cr"
    doliteral_ 13
    rcall EMIT
    rcall TEN
    rjmp EMIT
    
; ( -- 32 )
; put ascii code of the blank/space character to the stack

forthword_ BL, 0, "bl"
    doliteral_ 32
    ret
    
; ( -- f )
; refills the input buffer
forthword_ REFILL, 0, "refill"
    call DODEFER
    .word USER_REFILL
    addr_ FETCHU
    addr_ STOREU

; ( addr n --  )
; reads string from flash and prints it
forthword_ ITYPE, 0, "itype"
    rcall BTOW        ; ( addr len/2 rem )
    rcall TO_B        ; ( addr len/2 ) B: rem
    rcall SWAP        ; ( len/2 addr )
    twostar_          ; ( len/2 addr*2 )
    rcall TO_A        ; ( len/2 ) A: addr*2
    ; begin
PFA_ITYPE1:
    ; ?while
    dupzerosense_
    breq PFA_ITYPE2
    rcall AFETCHI     ; ( len c1c2 )
    aplus_            ; a+
    dup_              ; ( len c1c2 c1c2 )
    rcall EMIT        ; ( len c1c2 )
    highbyte_         ; ( len c2c1 )
    rcall EMIT        ; ( len )
    oneminus_         ; ( len-1 )
    ; repeat
    rjmp PFA_ITYPE1

PFA_ITYPE2:
    drop_             ; ( )
    cp bl, zerol      ; ( )
    breq PFA_ITYPE3
      rcall AFETCHI
      rjmp EMIT
PFA_ITYPE3:
    ret
    
; ( addr n -- )
; print a RAM based string
forthword_ TYPE, 0, "type"
    niptoa_          ; ( n ) A: addr
PFA_TYPE1:
    dupzerosense_
    breq PFA_TYPE2
    rcall ACFETCH    ; ( n c )
    aplus_           ; a+
    rcall EMIT       ; ( n )
    oneminus_        ; ( n-1 )
    rjmp PFA_TYPE1
PFA_TYPE2:
    rjmp DROP
    

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ( addr1 u1 n -- addr2 u2 )
; String
; adjust string from addr1 to addr1+n, reduce length from u1 to u2 by n
forthword_ SLASHSTRING, 0, "/$"
    rcall OVER    ; ( -- addr1 u1 n u1 )
    call MIN      ; ( -- addr1 u1 n|u1 )
    rcall ROT     ; ( -- u1 n addr1 )
    rcall OVER    ; ( -- u1 n addr1 n )
    rcall PLUS    ; ( -- u1 n addr2 )
    rcall RROT    ; ( -- addr2 u1 n )
    rjmp MINUS    ; ( -- addr2 u2)



; ( addr1 n1 c -- n2 )
; String
; skips leading occurances in string at addr1/n1 leaving n2 as an index pointing to the 1st non-c character

forthword_ CSKIP, 0, "cskip"
    rcall TO_B           ; ( addr1 n1 ) B: c
    niptoa_              ; ( n1 ) A: addr1
    dup_                 ; ( n1 n1 )
PFA_CSKIP1:
    dupzerosense_        ; ( n1 n')
    breq PFA_CSKIP2
    rcall ACFETCH        ; ( n1 n' c' )
    aplus_
    cp tosl, bl          ; ( n1 n' c' )
    drop_                ; ( n1 n' )
    brne PFA_CSKIP2
    oneminus_
    rjmp PFA_CSKIP1

PFA_CSKIP2:
    rjmp MINUS           ; ( n1-n' )
    

; ( addr1 n1 c -- addr1 n2 )
; String
; Scan string at addr1/n1 for the first occurance of c, leaving addr1 n2,
; char at n2 is first c character

forthword_ CSCAN, 0, "cscan"
    rcall TO_B           ; ( addr1 n1 ) B: c
    rcall OVER           ; ( addr1 n1 addr1 )
    rcall TO_A           ; ( addr1 n1 ) A: addr1
    dup_                 ; ( addr1 n1 n1 )
PFA_CSCAN1:
    dupzerosense_        ; ( addr1 n1 n')
    breq PFA_CSCAN2
    rcall ACFETCH        ; ( addr1 n1 n' c' )
    aplus_
    cp tosl, bl          ; ( addr1 n1 n' c' )
    drop_                ; ( addr1 n1 n' )
    breq PFA_CSCAN2
    oneminus_
    rjmp PFA_CSCAN1

PFA_CSCAN2:
    rjmp MINUS           ; ( addr1 n1-n' )
    



; ( r-addr r-len f-addr f-len --  f)
; Memory
; compares string in RAM with string in flash
forthword_ ICOMPARE, 0, "icmp"
    rcall TO_B               ; ( r-addr r-len f-addr)  B: f-len
    rcall OVER               ; ( r-addr r-len f-addr r-len)
    rcall B_                 ; ( r-addr r-len f-addr r-len f-len )
    ; check if strings are same length
    rcall NOTEQUAL           ; ( r-addr r-len f-addr flag )
    ; if
    zerosense_               ; ( r-addr r-len f-addr )
    breq PFA_ICOMPARE_SAMELEN

    ; strings are not the same length
    dnip_
    zerotos_                 ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_SAMELEN:
    twostar_                 ; ( r-addr len f-addr*2 )
    rcall TO_A               ; ( r-addr len ) A: f-addr
    oneplus_                 ; ( r-addr len+1 )
    twoslash_                ; ( r-addr (len+1)/2 )

    ; begin
PFA_ICOMPARE_LOOP:
    copytob_                 ; ( r-addr len ) B: len
    ; while
    zerosense_               ; ( r-addr )
    breq PFA_ICOMPARE_DONE

    dup_                     ; ( r-addr r-addr )
    rcall FETCH              ; ( r-addr r-cc )
    rcall AFETCHI            ; ( r-addr r-cc f-cc )
    aplus_
    ; flash strings are zero-padded at the last cell
    ; that means: if the flash cell is less $0100, than mask the
    ; high byte in the ram cell
    dup_                     ; ( r-addr r-cc f-cc f-cc )
    doliteral_ 0x100         ; ( r-addr r-cc f-cc f-cc 0x100 )
    rcall ULESS              ; ( r-addr r-cc f-cc flag)
    ; if
    zerosense_
    breq PFA_ICOMPARE_LASTCELL
    rcall SWAP               ; ( r-addr f-cc r-cc)
    zerohigh_                ; ( r-addr f-cc 0:r-cc)
    ; then
PFA_ICOMPARE_LASTCELL:
    rcall NOTEQUAL           ; ( r-addr flag)
    ; if
    zerosense_               ; ( r-addr )
    breq PFA_ICOMPARE_NEXTLOOP

    ; strings don't match
    zerotos_                 ; ( 0 )
    ret

    ; then
PFA_ICOMPARE_NEXTLOOP:
    twoplus_                 ; ( r-addr+2 )
    rcall B_                 ; ( r-addr+2 len )
    oneminus_                ; ( r-addr+2 len-1 )

    ; repeat
    rjmp PFA_ICOMPARE_LOOP

PFA_ICOMPARE_DONE:
    ; strings are the same
    drop_
    rjmp TRUE
    
    
; ( addr -- addr+1 n )
; Tools
; get string address and length information out of a counted string in flash
forthword_ ICOUNT, 0, "$l"
    dup_
    oneplus_
    rcall SWAP
    rjmp FETCHI


; ( -- srcaddr len  )
; String
; Adjust the source addr using >in.
forthword_ SRCIN, 0, "srcin"
    rcall STIB         ; ( srcaddr len )
    rcall G_IN           ; ( srcaddr len ginaddr )
    rcall FETCH          ; ( srcaddr len gin)
    rjmp SLASHSTRING     ; ( srcaddr' len' )
    

; ( -- )
; Input
; skip space in input source.
forthword_ SKIPBL, 0, "skipbl"
    rcall SRCIN          ; ( srcaddr len )
    rcall BL             ; ( srcaddr' len' c )
    rcall CSKIP          ; ( n2 )

    ; adjust >IN
    rcall G_IN           ; ( n2 ginaddr )
    rjmp PLUSSTORE       ; (  )


; ( char "ccc<char>" -- c-addr u )
; String
; in input buffer parse ccc delimited string by the delimiter char.
forthword_ PARSE, 0, "parse"
    rcall SRCIN      ; ( -- c addr len)
    rcall ROT        ; ( -- addr' len' c)
    rcall CSCAN      ; ( -- len'')
    dup_             ; ( -- addr' len'' len'')
    oneplus_         ; ( -- addr' len'' len''+1 )
    rcall G_IN       ; ( -- addr' len'' len''+1 >in)
    rjmp PLUSSTORE   ; ( -- addr' len'')


; ( "<name>" -- c-addr len )
; String
; In the SOURCE buffer parse whitespace delimited string. Returns string address within SOURCE.
forthword_ PNAME, 0, "pname"   
    rcall SKIPBL
    rcall BL
    rjmp PARSE
            
            
; ( c-addr len wid -- [ 0 | nfa ] )
; find a word in the dictionary
; 0 if word not found in dictionary
; nfa if word is found
forthword_ FINDNFA, 0, "findnfa"
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
    dupzerosense_               ; ( c-addr len nfa )
    brne FINDNFA_CHECK
      ; terminating, 0 found
      dnip_                     ; ( 0 )
      ret

FINDNFA_CHECK:
    rcall TO_R                  ; ( c-addr len ) (R: nfa )
    rcall TWOOVER               ; ( c-addr len c-addr len )
    rcall R_FETCH               ; ( c-addr len c-addr len nfa )
    rcall ICOUNT                ; ( c-addr len c-addr len f-addr len )
    zerohigh_                   ; ( c-addr len c-addr len f-addr 0:len )
    rcall ICOMPARE              ; ( c-addr len flag )
    ; <if>
    zerosense_
    breq FINDNFA_NEXT
      ; we found the string
      rcall DDROP               ; ( )
      rcall R_FROM              ; ( nfa )
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
forthword_ FINDROOT, 0, "froot"
    ; put root wid on stack
    doliteral_  pm(VE_WIPE) 
    ; search root
    rjmp FINDNFA_BEGIN

; ( addr len idx -- 0 | nfa )
FINDWI:
      twostar_          ; ( addr len idx*2)
      rcall CONTEXT     ; ( addr len idx*2 context )
      rcall PLUS        ; ( addr len context' )
      rcall FETCH       ; ( addr len wid )
      rjmp FINDNFA      ; ( nfa)

FINDLAST:
      drop_
      rjmp FINDROOT
      
; ( addr len --  0 | nfa )
; Tools
; search wordlists for the name from string addr/len
forthword_ FINDW, 0, "findw"
    ; loop through context slots to find a word
    ; start with last slot
    rcall CONTEXT       ; ( addr len context )
    ; get context index and use as counter
    oneminus_           ; ( addr len context-1 )
    rcall CFETCH        ; ( addr len idx )
FINDW_BEGIN:
    dupzerosense_
    brmi FINDLAST       ; ( addr len idx )
      rcall TO_R        ; ( addr len ) ( R: idx )
      rcall TWOOVER     ; ( addr len addr len ) ( R: idx )
      ; get next wordlist in array
      rcall R_FROM      ; ( addr len addr len idx ) ( R: )
      dup_r_            ; ( addr len addr len idx ) ( R: idx)
      rcall FINDWI
      rcall R_FROM      ; ( addr len nfa idx ) ( R: )
      rcall SWAP        ; ( addr len idx nfa )
      dupzerosense_
      ; end loop if word found
      breq FINDW_NEXT
        dnip_           ; ( addr nfa )
        nip_            ; ( nfa )
        ret
        
FINDW_NEXT:
      drop_             ; ( addr len idx )
      ; decrement index
      oneminus_         ; ( addr len idx-1 )
      rjmp FINDW_BEGIN

; ( -- addr len )
; String
; runtime portion of sliteral
; string literal is located starting at return cell
; return cell is string length
; string then occupies the following cells: c2c1 c4c3 ...

forthword_ DOSLIT, 0, "(slit)"

  ; get return address to get string length, calculate string address start,
  ; and calculate return address
  rcall R_FROM   ; ( raddr )  (R: raddr -- )
  dup_           ; ( raddr raddr )
  ; get the string length
  rcall FETCHI   ; ( raddr slength )
  rcall OVER     ; ( raddr slength raddr )
  ; calculate string address
  oneplus_       ; ( raddr slength straddr )
  ; calculate the number of words to skip over for return address
  rcall FLIP     ; ( straddr slength raddr )
  rcall OVER     ; ( straddr slength raddr slength)
  rcall WALIGN   ; ( straddr slength raddr k/2 )
  rcall PLUS     ; ( straddr slength raddr+k/2 )
  ; also skip string length
  oneplus_       ; ( straddr slength raddr+k/2+1 )
  rjmp EXEC


; ( -- ) (R: i*x - j*x )
; System
; interpret input word by word.
forthword_ INTERPRET, 0, "interp"
    ; begin
    rcall PNAME         ; ( -- addr len )

    ; ?while
    dupzerosense_       ; ( -- addr len )
    breq INTERPRET_END

    rcall RECX          ; ( )
    call QSTACK         ; ( )

    ; repeat
    rjmp INTERPRET

INTERPRET_END:
    rjmp DDROP

; ( addr len -- )
; System
; recognize and execute name of word in ram using recognizer list.
forthword_ RECX, 0, "recx"
    rcall TWOOVER         ; ( addr len addr len  )
    ; put addr and len on return stack so that
    ; stack can be manipulated by interpreted word
    rcall TO_R            ; ( addr len addr ) (R: len )
    rcall TO_R            ; ( addr len ) (R: len addr )
    ; test if its a word
    rcall REC_WORD        ; ( flag ) ( R: len addr )
    rcall TO_A            ; ( ) A: flag
    rcall R_FROM          ; ( addr ) (R: n len )
    rcall R_FROM          ; ( addr len ) (R: n )
    rcall A_              ; ( addr len flag ) A:  
    ; <if>
    zerosense_            ; ( addr len )
    brne RECX_DONE
    rcall TWOOVER         ; ( addr len addr len  )
    rcall TO_R            ; ( addr len addr ) (R: len )
    rcall TO_R            ; ( addr len ) (R: len addr )
    ; test if its a number
    rcall REC_NUM         ; ( flag ) ( R: len addr )
    rcall TO_A            ; ( ) A: flag
    rcall R_FROM          ; ( addr ) (R: n len )
    rcall R_FROM          ; ( addr len ) (R: n )
    rcall A_              ; ( addr len flag ) A:  
    zerosense_            ; ( addr len )
    brne RECX_DONE
    ; not recognized so throw
    rcall CR
    ; print unrecognized word
    rcall TYPE
    rcall DOSLIT
    ascii_ " Unknown!"
    jmp THROW
    
RECX_DONE:
    rjmp DDROP           ; ( )


; ( addr len -- n f )
; Interpreter
; recognizer for integer numbers
forthword_ REC_NUM, 0, "rec#"
    ; try converting to a number
    call NUMBER              ; ( [n] f )
    zerosense_
    breq PFA_REC_NONUMBER

    rcall STATEFETCH         ; ( n state )
    zerosense_
    breq PFA_REC_NUMBER_OK
    call LIT
PFA_REC_NUMBER_OK:
    rjmp TRUE                ; ( n true )

PFA_REC_NONUMBER:
    rjmp ZERO                ; ( 0 )

; ( addr len -- f )
; Interpreter
; recognize a word in the dictionary
; will 
forthword_ REC_WORD, 0, "recw"

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
    rcall STATEFETCH
    zerosense_
    brne CHECK_IMMEDIATE
        ; word can only be used in compile state
        call DOSLIT
        ascii_ " Only Compile!"
        jmp THROW
    

CHECK_IMMEDIATE:
	; either compile or EXEC the XT
    ; check if word is immediate: bit 7 is clear if immediate
	sbrs tosh, IMMEDIATE_EN_FB ; skip next instruction if bit 7 is set: not immediate word
	    ; flag is 0: always EXEC
	    rjmp REC_WORD_EXECUTE

REC_WORD_CHECKSTATE:
        ; check state
	    rcall STATEFETCH
	    zerosense_
	    breq REC_WORD_EXECUTE
        ; in compile mode so compile xt
 	    call COMPILEXT
	    rjmp TRUE
REC_WORD_EXECUTE:
    drop_
    ; state is zero, EXEC xt
    rcall EXEC
REC_WORD_OK:
    rjmp TRUE

    
; ( -- )
; System
; check if a ready prompt is required
forthword_ QP_RD, 0, "?prd"
    rcall STATEFETCH
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
; wait for the input buffer to be refilled then interpret
forthword_ DOREFILL, 0, "(refill)"
  ; if just starting new input line then check prompt and reset input position
  rcall G_IN               ; ( >inaddr )
  rcall FETCH              ; ( >in )
  zerosense_               ; ( )
  breq DOREFILL_do
    rcall G_IN             ; ( >inaddr )
    rcall ZEROSTORE        ; (  )
    ; reset input buffer cursor position
    rcall SHARPTIB         ; ( #tibaddr )
    rcall ZEROSTORE        ; ( )
    rjmp QP_RD
    
DOREFILL_do:
  ; refill the input buffer
  rcall REFILL              ; ( f )
  ; <if>
  zerosense_
  breq DOREFILL_end
  
    ; there is something in the input buffer, try interpreting it      
    rjmp DOINTERPRET
DOREFILL_end:
  ; <then>
  ret      
    
; ( -- )
; Multitasking
; Fetch pause vector and EXEC it. may make a context/task switch
forthword_ PAUSE, 0, "pause"
    call DODEFER
    .word USER_PAUSE
    addr_ FETCHU
    addr_ STOREU

; ( -- )
; System
; main loop - iterate through scheduled tasks 
forthword_ DOTASKS, 0, "(tasks)"
    ; <begin>
      rcall DOREFILL
      rcall PAUSE
    ; <again>
    rjmp DOTASKS

; ( -- )
; System
; reset stacks and start over again
forthword_ QUIT, 0, "quit"
    ; reset data stack
    call SP0
    rcall SP_STORE
    ; reset return stack pointers
    call RP0
    rcall RP_STORE
    rcall SMUDGE
    rcall ZEROSTORE
    ; restore dp from eeprom
    rcall DPFETCHE
    
    ; switch to interpret mode
    rcall LBRACKET
    
    rjmp DOTASKS
    
        
; ( -- )
; System
; interpret what is in the input buffer
forthword_ DOINTERPRET, 0, "(interp)"
      ; input buffer has something so try to interpret it
      ; setup exception handling in case of error
  doliteral_ pm(INTERPRET)
  call CATCH
  ; check to make sure no throws
  rcall QDUP
  ; <if>
  zerosense_
  breq DOINTERPRET_ok
    rcall PROMPTERROR
	rjmp QUIT
	
  ; <then>
DOINTERPRET_ok:
  ; make sure in buf pointer is not at zero
  rcall G_IN               ; ( 1 >inaddr )
  rcall ONEPLUSSTORE       ; ( )
  rjmp PROMPTOK

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTRDY, 0, ".>"
    rcall CR
    rcall DOSLIT
    ascii_ "> "
    rjmp ITYPE

; ( -- )
; System
; send the READY prompt to the command line
forthword_ PROMPTCOMPILE, 0, ".:"
    rcall CR
    rcall DOSLIT
    ascii_ ": "
    rjmp ITYPE

; ( -- )
; System
; send the OK prompt to the command line
forthword_ PROMPTOK, 0, ".ok"
    rcall DOSLIT
    ascii_ " ok"
    rjmp ITYPE

; ( n -- )
; System
; process the error prompt
forthword_ PROMPTERROR, 0, ".??"
	drop_
	rcall DOSLIT
	ascii_ " ?? "
    rjmp ITYPE


; ( -- flag )
; System
; receive a string of at most tibsize characters or cr/lf detected.
; flag will be true if input buffer needs to be processed ie interpreted
; flag will be false if more input needed

forthword_ ACCEPT, 0, "accept"
    ; <begin>
    rcall KEY        ; ( k )
    dupzerosense_    ; ( k )
    brne ACCEPT_haskey
    ret

ACCEPT_haskey:
    ; check for EOL
    cpi tosl, 10
    ; <if>
    brne ACCEPT_checkreturn
    ret

ACCEPT_checkreturn:
    cpi tosl, 13     ; ( k k 13 )
    ; <if>
    brne ACCEPT_checkdelete
    ret

ACCEPT_checkdelete:
    ; check delete
    cpi tosl, 127      ; ( k k 8 )
    ; <if>
    brne ACCEPT_checkcontrol
    ; delete previous character
    ; check beginning of line
    ; if cursor is 0 then at begining of input
    rcall SHARPTIB
    rcall FETCH              ; ( k #tib )
    ; <if>
    zerosense_               ; ( k )
    brne ACCEPT_dodelete
    ; at the beginning of the line, ignore this character
    drop_
    rjmp ZERO

ACCEPT_dodelete:
    andi tosl, 8
    dup_                 ; ( k k )
    rcall EMIT           ; ( k )
    rcall BL             ; ( k bl )
    rcall EMIT           ; ( k )
    rcall EMIT           ; ( )
    ; decrease cursor position by one
    rcall TRUE           ; ( -1 )
    rcall SHARPTIB       ; ( -1 #tibaddr )
    rcall PLUSSTORE       ; ( )
    rjmp ACCEPT_checkmaxinput

ACCEPT_checkcontrol:
    ; check for remaining control characters, replace them with blank
    cpi tosl, 32         ; ( k k bl )
    brge ACCEPT_echo
    drop_
    ; replace control key with a space
    rcall BL

ACCEPT_echo:
    ; emit the key
    dup_                 ; ( k k)
    rcall EMIT           ; ( k)
    ; now store the key in the input buffer
    rcall STIB      ; ( k tibaddr #tib)
    rcall PLUS           ; ( k tibaddr+#tib ) 
    rcall CSTORE         ; ( )

    rcall SHARPTIB       ; ( 1 #tibaddr )
    rcall ONEPLUSSTORE   ; ( )

ACCEPT_checkmaxinput:
    ; check if max number of char input
    doliteral_ TIBSIZE   ; ( tibsize )
    rcall SHARPTIB       ; ( tibsize #tibaddr )
    rcall FETCH          ; ( tibsize #tib)
    rcall MINUS          ; ( tibsize-#tib)
    ; <if>
    zerosense_           ; ( )
    breq ACCEPT_atmaxinput
    rjmp ZERO
    
ACCEPT_atmaxinput:
    ; max number of characters in input buffer so have it processed
    rjmp TRUE

; ( -- f )
; System
; refills the input buffer - flag is true if buffer has something
forthword_ REFILLTIB, 0, "reftib"
    rcall ACCEPT               ; ( f )
    dupzerosense_              ; ( f )
    breq REFILLTIB_exit
    rjmp CR
    
REFILLTIB_exit:
    ret
