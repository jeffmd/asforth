
( -- n )
\ Tools
\ Amount of available RAM (incl. PAD)
: unused
    sp0 here -
;
    


( -- )
( System )
( Arduino pin 13 portB bit 5 debug.)

: dbg- 
  [
    4 5 sbi,
    5 5 cbi,
  ]
; 

: dbg+
  [
    4 5  sbi,
    5 5  sbi,
  ]
;

( val -- )
\ output hex value in FFFF format
: .$ hex <# # # # # #> type space decimal ;

( addr1 cnt -- addr2)
: dmp
 over .$ [char] : emit space
 begin
   ?while 1- swap dup @i .$ 1+ swap
 repeat
 drop
;

( -- ) 
\ Tools
\ prints a list of all (visible) words in the dictionary
: words
    0                      ( 0 )
    wid
    @e
    begin
      ?dup                   ( cnt addr addr )
    while                    ( cnt addr ) \ is nfa = counted string
      dup                    ( cnt addr addr )
      $l $FF and             ( cnt addr addr n ) \ mask immediate bit
      itype space            ( cnt addr )
      nfa>lfa                ( cnt lfa )
      @i                     ( cnt addr )
      swap                   ( addr cnt )
      1+                     ( addr cnt+1 )
      swap                   ( cnt+1 addr )
    repeat 

    cr .
;

( addr -- )
\ Tools
\ print the contents at ram addr
: ? @ . ;

( n -- )
\ Tools
\ add an Interrupt Service Routine to the ISR vector table
\ n is the address of the table entry
\ only need to write the address 
\ jmp instruction is already in vector table
: isr 1+ ' swap !i ;

( bbb reg -- )
\ tools
\ set the bits of reg defined by bit pattern in bbb
: rbs :a c@ or ac! ;

( bbb reg --- )
\ tools
\ clear the bits of reg defined by bit pattern in bbb
: rbc >a not ac@ and ac! ;
