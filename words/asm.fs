\ asm.fs - Atmel atmega assembler

\ AvrAsm - assembler Atmega chips, Lubos Pekny, www.forth.cz
\ Library for amforth 3.0 mFC 1.0

\ V.1.2v, 18.08.2014, modified for asforth
\ V.1.1v, 29.01.2009, add vocabulary only

\ V.1.1, 15.05.2008, tested on atmega32, amforth 2.7
\ - change reg tosl,tosh in Test AvrAsm (loadtos, savetos)
\ - change <label to <labelr
\ - vector of labels, 20 bytes in RAM, example

\ V.1.0, 07.02.2008, tested on atmega168, amforth 2.6

only
vocabulary ASM
ASM definitions

  \ Operands Rd,Rr
: Rd,Rr,    ( Rd Rr opcode mask -- xxxz.xxrd.dddd.rrrr )
    >r >r                              \ -- Rd Rr | -- mask opcode 
    $1F and dup 5 << or $20F and       \ -- Rd r00000rrrr
    swap 4 << $1F0 and                 \ -- rr 0ddddd0000
    or r> r>  mask!                    \ -- ddrr opcode mask mask!
    dup $FC07 and $9000 = if $EFFF and then , ; \ if Z or Y then z=0 


  \ Operand Rd
: Rd,       ( Rd opcode mask -- xxxx.xxxd.dddd.xxxx )
    >r >r                          \ -- Rd | -- mask opcode 
    4 << $1F0 and                  \ -- 0ddddd0000
    r> r> mask! , ;                \ dd opcode mask mask! to flash
    
  \ Operands Rd,Rr,constant 6bit
: Rd,Rr+q,  ( Rd Rr k6 opcode mask -- xxkx.kkxd.dddd.rkkk )
    >r >r                           \ -- Rd Rr k6 | -- mask opcode
    $3F and dup 7 <<                \ -- Rd Rr k6 xkkkkkkxxxxxxx
    dup $1000 and 1 << or or $2C07 and  \ -- Rd Rr kxkkxxxxxxxkkk
    rot 4 << $1F0 and               \ -- Rr kk ddddd0000
    or swap 8 and                   \ -- kkdd rxxx
    or r> r> mask! , ;              \ kkddrr opcode mask mask! to flash


  \ Operands Rw pair,constant 6bit
: Rw,k,     ( Rw k6 opcode mask -- xxxx.xxxx.kkww.kkkk )
    >r >r                           \ -- Rw k6 | -- mask opcode
    $3F and dup 2 << $C0 and        \ -- Rw k6 kk000000
    swap $F and or                  \ -- Rw kk00kkkk
    swap 4 << $30 and               \ -- kk ww0000
    or r> r> mask! , ;              \ kkww opcode mask mask! to flash
    
  \ Operands Rd,P-port
: Rd,P,     ( Rd P opcode mask -- xxxx.xPPd.dddd.PPPP )
    >r >r                           \ -- Rd P | -- mask opcode 
    $3F and dup 5 << or $60F and    \ -- Rd PP00000PPPP
    swap 4 << $1F0 and              \ -- PP 00ddddd0000
    or r> r> mask! , ;              \ ddPP opcode mask mask! to flash


$00 con Z
$01 con Z+
$02 con -Z
$08 con Y
$09 con Y+
$0A con -Y
$0C con X
$0D con X+
$0E con -X

24 con TOSL
25 con TOSH

\ hex numbers that get used a lot
: $FC00 $FC00 ;
: $FF88 $FF88 ;
: $F000 $F000 ;
: $FE0F $FE0F ;
: $FE08 $FE08 ;


: movw,   2/ swap        \ R0:1,R2:3,R4:5,..R30:31
          2/ swap        \ 0 2 movw, R0:1<--R2:3
          $0100 $FF00  Rd,Rr, ;  ( Rd Rr -- )
: mul,    $9C00 $FC00  Rd,Rr, ;  \ 2 3 mul,
: muls,   $0200 $FF00  Rd,Rr, ;
: mulsu,  $0300 $FF88  Rd,Rr, ;
: fmul,   $0308 $FF88  Rd,Rr, ;
: fmuls,  $0380 $FF88  Rd,Rr, ;
: fmulsu, $0388 $FF88  Rd,Rr, ;
: cpc,    $0400 $FC00  Rd,Rr, ;
: sbc,    $0800 $FC00  Rd,Rr, ;
: add,    $0C00 $FC00  Rd,Rr, ;
: cpse,   $1000 $FC00  Rd,Rr, ;
: cp,     $1400 $FC00  Rd,Rr, ;
: sub,    $1800 $FC00  Rd,Rr, ;
: adc,    $1C00 $FC00  Rd,Rr, ;
: and,    $2000 $FC00  Rd,Rr, ;
: eor,    $2400 $FC00  Rd,Rr, ;
: or,     $2800 $FC00  Rd,Rr, ;
: mov,    $2C00 $FC00  Rd,Rr, ;  \ 2 3 mov,  R2<--R3

: cpi,    $3000 $F000  Rd,k,  ;  ( Rd k8 -- )
: sbci,   $4000 $F000  Rd,k,  ;
: subi,   $5000 $F000  Rd,k,  ;
: ori,    $6000 $F000  Rd,k,  ;
: sbr,    ori, ;
: andi,   $7000 $F000  Rd,k,  ;
: cbr,    not andi, ;


: lsl,    dup add, ;           ( Rd -- )
: rol,    dup adc, ;
: tst,    dup and, ; 
: clr,    dup eor, ;
: ser,    $FF  ldi, ;

: pop,    $900F $FE0F  Rd, ;     ( Rd -- ) \ 2 pop,
: push,   $920F $FE0F  Rd, ;
: com,    $9400 $FE0F  Rd, ;
: neg,    $9401 $FE0F  Rd, ;
: swap,   $9402 $FE0F  Rd, ;
: inc,    $9403 $FE0F  Rd, ; 
: asr,    $9405 $FE0F  Rd, ;
: lsr,    $9406 $FE0F  Rd, ;
: ror,    $9407 $FE0F  Rd, ;
: bset,   $9408 $FF8F  Rd, ;
: bclr,   $9488 $FF8F  Rd, ;
: dec,    $940A $FE0F  Rd, ;


: sleep,  $9588 , ;
: break,  $9598 , ;
: wdr,    $95A8 , ;

: clc,    $9488 , ;
: clh,    $94D8 , ;
: cln,    $94A8 , ;
: cls,    $94C8 , ;
: clt,    $94E8 , ;
: clv,    $94B8 , ;
: clz,    $9498 , ;
: sec,    $9408 , ;
: seh,    $9458 , ;
: sen,    $9428 , ;
: ses,    $9448 , ;
: set,    $9468 , ;
: sev,    $9438 , ;
: sez,    $9418 , ;

: adiw,   $9600 $FF00  Rw,k, ;   ( Rw k6 -- ) \ 3 3F adiw, ZLH=ZLH+#3F
: sbiw,   $9700 $FF00  Rw,k, ;

: in,     $B000 $F800  Rd,P, ;   ( Rd P -- )
: out,    swap
          $B800 $F800  Rd,P, ;   ( P Rr -- )

: bld,    $F800 $FE08  Rd,Rr, ;  ( Rd b -- )
: bst,    $FA00 $FE08  Rd,Rr, ;
: sbrc,   $FC00 $FE08  Rd,Rr, ;
: sbrs,   $FE00 $FE08  Rd,Rr, ;


: brbc,   $F400 $FC00   P,b, ;   ( k7 b -- )
: brbs,   $F000 $FC00   P,b, ;
: brcc,   0 brbc, ;            ( k7 )
: brcs,   0 brbs, ;
: brsh,   0 brbc, ;
: brlo,   0 brbs, ;
: brne,   1 brbc, ;
: breq,   1 brbs, ;
: brpl,   2 brbc, ;
: brmi,   2 brbs, ;
: brvc,   3 brbc, ;
: brvs,   3 brbs, ;
: brge,   4 brbc, ;
: brlt,   4 brbs, ;
: brhc,   5 brbc, ;
: brhs,   5 brbs, ;
: brtc,   6 brbc, ;
: brts,   6 brbs, ;
: brid,   7 brbc, ;
: brie,   7 brbs, ;
