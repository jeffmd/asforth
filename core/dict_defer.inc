; dict_defer.inc
; words that support defer

    
; ( -- addr )
; System
; runtime part of user
; get the address of the user var which is located at the return address of the caller
forthword_ DOUSER, DIS_CALL_OPT, 6, "(user)"
    pushtos
    pop ZH
    pop ZL
    rcall ZDFETCHI
    ; calculate user address
    add tosl, upl
    adc tosh, uph
    ret

; ( defaddr -- deferval defaddr )
; System
; get defer contents
forthword_ DEFGET, 0, 4, "defg"
    dup_                ; ( addr addr )
    ; get address of defer
    rcall FETCHI        ; ( addr valaddr )
    rjmp SWAP          ; ( valaddr addr )
 
; ( addr -- n )
; System
; execute the defer fetch
forthword_ DEFX, 0, 4, "defx"
    rcall DEFGET
    ; get the fetch Xt pointer which is one word after the data
    oneplus_
    rcall FETCHI        ; ( valaddr xtfetch )
    ; EXEC the fetch
    rjmp EXEC        ; ( n )

; ( -- n )
; System
; runtime of val
; must use call
forthword_ DOVALUE, DIS_CALL_OPT, 5, "(val)"
; must be called, don't use jmp
    ; Top of return stack has return address of caller
    ; return address is actually the address of the value
    ; won't be returning to this address
    rcall R_FROM        ; ( raddr ) (R: raddr -- )
    rjmp DEFX           ; ( n )

; ( i*x -- j*x )
; System
; runtime of defer
forthword_ DODEFER, DIS_CALL_OPT, 5, "(def)"
    ; top of return stack has return address of caller
    ; return address is data for defer so pop it onto data stack
    ; will not be returning to the caller
    rcall R_FROM         ; ( raddr ) (R: raddr -- )
    rcall DEFX           ; ( n )
    ; EXEC the defer
    rjmp EXEC

; ( xt1 -- xt2 )
; System
; returns the XT associated with the given XT
forthword_ DEFERFETCH, 0, 4, "def@"
    ; skip passed call to get to data field
    twoplus_                 ; ( xt1+2 )
    rjmp DEFX

; ( xt1 xt2 -- )
; System
; stores xt1 as the xt to be EXECd when xt2 is called
forthword_ DEFERSTORE, 0, 4, "def!"
    ; skip past call to get to data
    twoplus_                 ; ( xt1 xt2+2 )
    rcall DEFGET              ; ( st1 defaddr xt2+2 )
    ; skip past data address and deferfetch section to get
    ; to deferstore section
    twoplus_                 ; ( xt1 defaddr xt2+4 )
    ; get defered store xt address
    rcall FETCHI              ; ( xt1 defaddr xtstore )
    rjmp EXEC

; ( n -- ) (R: IP -- IP+2)
; Tools
; runtime portion of to
; must be called
forthword_ DOTO, DIS_CALL_OPT, 4, "(to)"
    rcall R_FETCHPLUS    ; ( n -- n raddr ) (R: raddr -- raddr+1 )
    ; next cell contains address of deferred word
    rcall FETCHI         ; ( n defxt )
    rjmp DEFERSTORE
