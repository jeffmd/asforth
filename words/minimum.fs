\ minimum.fs Forth words that make up minimum forth vocabulary


( n min max -- f)
\ check if n is within min..max
: within
    over - >a - a u<
;

\ increment a cvar by one.  If val > max then set flag to true.
: 1+c!mx ( maxval cvar -- flag )
  nip>b >a ac@ 1+ dup b > if 0: then dup ac! 0= 
;

( c<name> -- ) 
\ Compiler
\ creates a defer vector which is kept in eeprom.
: edefer
    (create)
    cur@ !e
    compile (def)

    edp             ( -- EDP )
    dup             ( -- EDP EDP )
    ,               ( -- EDP )
    ['] @e ,
    ['] !e ,
    \ increment EDP one cell then save it
    dcell+          ( -- EDP+2 )
    to edp
;

( c<name> -- ) 
\ Compiler
\ creates a RAM based defer vector
: rdefer
    (create)
    cur@ !e

    compile (def)

    here ,
    dcell allot

    ['] @ ,
    ['] ! ,
;


\ signed multiply n1 * n2 and division  with n3 with double
\ precision intermediate and remainder
: */mod  ( n1 n2 n3 -- rem quot )
    >r
    m*
    r>
    um/mod
;


\ signed multiply and division with double precision intermediate
: */ ( n1 n2 n3 -- n4 )
    */mod
    nip
;

\ divide n1 by n2. giving the quotient
: /  ( n1 n2 -- n3)
    /mod
    nip
;


\ divide n1 by n2 giving the remainder n3
: mod ( n1 n2 -- n3 )
    /mod
    drop
;


\ fill u bytes memory beginning at a-addr with character c
\ : fill  ( a-addr u c -- ) 
\    -rot           ( c a-addr u )
\    nip>a          ( c u ) ( A: a-addr )
\    begin
\    ?while
\      over         ( c u c )
\      ac!          ( c u )
\      a+
\      1-           ( c u-1 )
\    repeat
\    2drop
\ ;


\ emits a space (bl)
: space ( -- )
    bl emit
;

\ emits n space(s) (bl)
\ only accepts positive values
: spaces ( n -- )
    \ make sure a positive number
    dup 0> and
    begin
    ?while
      space
      1- 
    repeat
    drop
;

\ pointer to current write position
\ in the Pictured Numeric Output buffer
var hld


\ prepend character to pictured numeric output buffer
: hold ( c -- )
    -1 hld +!   
    hld @ c!
;

\ Address of the temporary scratch buffer.
: pad ( -- a-addr )
    here 20 +
;

\ initialize the pictured numeric output conversion process
: <# ( -- )
    pad hld !
;


\ pictured numeric output: convert one digit
: # ( u1 -- u2 )
    base@      ( u1 base )
    u/mod      ( rem u2 )
    swap       ( u2 rem )
    #h hold    ( u2 )
;

\ pictured numeric output: convert all digits until 0 (zero) is reached
: #s ( u -- 0 )
    #
    begin
    ?while
      #
    repeat
;


\ Pictured Numeric Output: convert PNO buffer into an string
: #> ( u1 -- addr count )
    drop hld @ pad over -
;

\ place a - in HLD if n is negative
: sign ( n -- )
    0< if [char] - hold then
;


\ singed PNO with cell numbers, right aligned in width w
: .r ( wantsign n w -- )
    >r   ( wantsign n ) ( R: w )
    <#
    #s   ( wantsign 0 )
    swap ( 0 wantsign )
    sign ( 0 )
    #>   ( addr len )
    r>   ( addr len w )  ( R: )
    over ( addr len w len )
    -    ( addr len spaces )
    spaces ( addr len )
    type  ( )
    space
;

\ unsigned PNO with single cell numbers
: u. ( u -- )
    0      ( n 0 ) \ want unsigned
    tuck   ( 0 n 0 )
    .r 
;


\ singed PNO with single cell numbers
: .  ( n -- )
    dup      ( n n )
    abs      ( n n' )
    0        ( n n' 0 ) \ not right aligned
    .r
;

\ stack dump
: .s  ( -- ) 
    sp@     ( limit ) \ setup limit
    dcell- 
    sp0     ( limit counter )
    begin 
    dcell-  ( limit counter-2 )
    2over   ( limit counter-2 limit counter-2 )
    <>      ( limit counter-2 flag )
    while 
      dup     ( limit counter-2 counter-2 )
      @       ( limit counter-2 val )
      u.      ( limit counter-2 )
    repeat   
    2drop
;

\ numbers that get used a lot

: 3 3 ;
: 4 4 ;
: 5 5 ;
: 6 6 ;
: 7 7 ;
: 8 8 ;
: 9 9 ;
: $FF $FF ;
: $F0 $F0 ;
: $0F $0F ;
: $FF00 $FF00 ;


( xt1 c<char> -- ) 
\ stores xt into defer or compiles code to do so at runtime
: is
    [compile] to
; immediate

( n c<name> -- )
\ add an Interrupt Service Routine to the ISR vector table
\ n is the address of the table entry
\ only need to write the address 
\ jmp instruction is already in vector table
: isr 1+ ' swap !i ;

( C: name -- )
\ start defining an Interrupt Service Routine
: :isr : compile (i:) ; immediate

( -- )
\ finish defining an Interrupt Service Routine
: ;isr compile (i;) [compile] ; ; :ic
