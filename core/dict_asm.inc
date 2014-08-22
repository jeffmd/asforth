; Based on:
; AvrAsm - assembler Atmega chips, Lubos Pekny, www.forth.cz
; Library for amforth 3.0 mFC 1.0

; Store src to dest thru mask, FF00 0551 0333 mask! u. FD11
;( dest1 src mask -- dest2 )
forthword_ MASKNOT, 0, 5, "mask!" 
    call ROT          ;( src mask dest1 )
    call OVER         ;( src mask dest1 mask )
    not_              ;( src mask dest1 !mask )
    call AND          ;( src mask dest1' )
    call RROT         ;( dest1' src mask )
    call AND          ;( dest1' src' )
    jmp OR            ;( dest1'|src' )


; Operands Rd,constant 8bit
; Rd,k,     ( Rd k8 opcode mask -- xxxx.kkkk.dddd.kkkk )
forthword_ RD_K, 0, 5, "Rd,k,"
    call TO_R            ;( Rd k8 opcode ) (R: mask )
    call TO_R            ;( Rd k8 ) (R: mask opcode )
    zerohigh_            ;( Rd 00k8 )
    dup_                 ;( Rd 00k8 00k8 )
    doliteral_ 4         ;( Rd 00k8 00k8 4 )
    call LSHIFT          ;( Rd 00k8 0000kkkk.kkkk0000 )
    call OR              ;( Rd 0000kkkk.kkkkkkkk )
    anditos_ 0x0f0f      ;( Rd 0000kkkk0000kkkk )
    call SWAP            ;( 0000kkkk0000kkkk Rd )
    swapnib_             ;( 0000kkkk0000kkkk Rd' )
    anditos_ 0x00f0      ;( kk dddd0000 )
    call OR              ;( 0000kkkkddddkkkk )
    call R_FROM          ;( 0kdk opcode )
    call R_FROM          ;( 0kdk opcode mask )
    rcall MASKNOT        ;( ops )
    jmp COMMA            ;( )

; load register with immediate value
; ( reg n -- )
; ex: 2 FF ldi,  R2<--#FF

forthword_ LDI, 0, 4, "ldi,"
    doliteral_ 0xE000     ;( reg n 0xe000 )
    doliteral_ 0xF000     ;( reg n 0xe000 0xf000 )
    rjmp  RD_K            ;( )

; Operands P-port,bit
; Assembler
; ( Port bit opcode mask -- xxxx.xxxx.PPPP.Pbbb )
forthword_ P_b_, 0, 4, "P,b,"
    call TO_R
    call TO_R     ; ( P b ) (R: -- mask opcode)
    anditos_ 7    ; ( P b )
    call SWAP     ; ( b P )
    doliteral_ 3  ; ( b P 3 )
    call LSHIFT   ; ( 0bbb PPPPP000)
    call OR       ; ( PPPPPbbb )
    call R_FROM   ; ( PPPPPbbb opcode )
    call R_FROM   ; ( PPPPPbbb opcode mask)
    rcall MASKNOT ; ( inst )
    jmp COMMA   ; PPbb opcode mask mask! to flash

; Clear bit in I/O port
; Assembler
;   ( Port bit -- )
forthword_ CBI_C, 0, 4, "cbi,"
    doliteral_ 0x9800
    doliteral_ 0xff00
    rjmp P_b_
 
; Set bit in I/O port
; Assembler
;   ( Port bit -- )
forthword_ SBI_C, 0, 4, "sbi,"
    doliteral_ 0x9a00
    doliteral_ 0xff00
    rjmp P_b_

; Skip next instruction if bit in I/O port is clear
; Assembler
;   ( Port bit -- )
forthword_ SBIC_C, 0, 5, "sbic,"
    doliteral_ 0x9900
    doliteral_ 0xff00
    rjmp P_b_

; Skip next instruction if bit in I/O port is set
; Assembler
;   ( Port bit -- )
forthword_ SBIS_C, 0, 5, "sbis,"
    doliteral_ 0x9b00
    doliteral_ 0xff00
    rjmp P_b_


; Compiler
; compile load top of stack with immediate value
; ( n -- )

forthword_ LIT, 0, 3, "lit"
    ; compile push tos
    doliteral_ 0x939a     ;( n 0x939a )
    call COMMA
    doliteral_ 0x938a     ;( n 0x938a )
    call COMMA
    doliteral_ tosl       ;( n tosl )
    call OVER             ;( n tosl n )
    rcall LDI             ;( n )
    call BYTESWAP         ;( n' )
    doliteral_ tosh       ;( n' tosh )
    call SWAP             ;( tosh n' )
    rjmp LDI              ;(  )

; Assembler
; compile assembler ret 
; ( -- )

forthword_ RET_C, 0, 4, "ret,"
    doliteral_ 0x9508     ;( 0x9508 )
    jmp COMMA
    
; compile rjmp
; ( distance -- )
forthword_ RJMP, 0, 5, "rjmp,"
    andi    tosh, 0x0f
    ori     tosh, 0xc0
    jmp     COMMA

; calc relative distance
; ( start dest -- start distance )
forthword_ RELDIST, 0, 7, "reldist"
    ; calculate relative distance in words from start to dest
    ; rel dist = dest - start
    call OVER            ; ( start dest start )
    call MINUS           ; ( start dist )
    oneminus_            ; ( start dist-1 )
    ret


; compile rjmp at start address
; ( start dest -- )
forthword_ RJMPC, 0, 5, "rjmpc"
    ; clear rcall/call flags
    cbr fflags, (1<<LAST_RCALL_FB)|(1<<LAST_CALL_FB)
    ; set disable tail optimize flag
    sbr fflags, 1<<DIS_CALL_OPT_FB
    ; calculate relative distance in words from start to dest
    rcall RELDIST
    ; compile a rjmp at first mark that jumps to the current DP
    andi    tosh, 0x0f
    ori     tosh, 0xc0   ; ( start rjmpdist )
    call SWAP            ; ( rjmpdist start )
    jmp STOREI           ; ( )

; compile rcall to xt at the current dictionary position (DP) if relative distance is in range
; otherwise do long call to xt.  If word flag indicates inlining then the word is inlined.
; ( xt flags -- )
forthword_ COMPILEXT, 0, 3, "cxt"
    ; default to tail call optimization enabled
    cbr fflags, 1<<DIS_CALL_OPT_FB
    ; check bit 2 of word high byte flags: if set then tail call optimization stays enabled
    sbrs tosh, DIS_CALL_OPT_FB
    ; bit 2 of high byte flag is clear so disable tail call optimization
    sbr fflags, 1<<DIS_CALL_OPT_FB
    ; if bit 0 of high byte flag is 0 then word is to be inlinned
    sbrc tosh, INLINE_OPT_FB
    rjmp DO_COMPILEXT
    drop_           ; ( xt )
    jmp INLINE

DO_COMPILEXT:
    drop_                ; ( xt )
    ; calculate relative distance in words from start to dest
    call DP              ; ( xt dp )
    call OVER            ; ( xt dp xt )

    rcall RELDIST        ; ( xt dp distance )
    nip_                 ; ( xt distance )
    ; is the relative distance in range
    call QRDIST          ; ( xt distance flag )
    zerosense_           ; ( xt distance )
    ; do relative call if flag is true
    brne DO_RCALLC
    drop_                ; ( xt )
    rjmp CALLC

DO_RCALLC:
    ; set compile flags: last instruction compiled was a rcall
    cbr fflags, 1<<LAST_CALL_FB ; clear call flag bit
    sbr fflags, 1<<LAST_RCALL_FB ; set rcall flag bit

    nip_                 ; ( distance )
    ; compile a rcall at the current DP
    andi    tosh, 0x0f
    ori     tosh, 0xd0   ; ( rcalldist )
    jmp COMMA            ; ( )

; compile call at start address
; ( xt -- )
forthword_ CALLC, 0, 5, "callc"
    ; set compile flags: last instruction compiled was a call
    cbr fflags, 1<<LAST_RCALL_FB ; clear rcall flag bit
    sbr fflags, 1<<LAST_CALL_FB ; set call flag bit

    doliteral_ 0x940e   ; ( xt 0x940e )
    call COMMA          ; ( xt )
    jmp COMMA

; branch 1 word if not zero
; ( -- )
forthword_ BRNZ1_COMPILE, IMMEDIATE_EN, 6, "brnz1,"
    ;compile brne 1w : F409
    doliteral_ 0xf409
    jmp COMMA

; dupzerosense inline
; ( f -- f )
forthword_ DUPZEROSENSE_COMPILE, IMMEDIATE_EN, 4, "?0?,"
    doliteral_ 0x9700
    jmp COMMA
