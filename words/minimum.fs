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
    current @
    !e
    compile (defer)

    edp             ( -- EDP )
    dup             ( -- EDP EDP )
    ,               ( -- EDP )
    ['] @e ,
    ['] !e ,
    \ increment EDP one cell then save it
    2+              ( -- EDP+2 )
    to edp
;

( c<name> -- ) 
\ Compiler
\ creates a RAM based defer vector
: rdefer
    (create)
    current @
    !e

    compile (defer)

    here ,
    2 allot

    ['] @ ,
    ['] ! ,
;

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
