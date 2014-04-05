
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

( bbb reg -- )
\ tools
\ set the bits of reg defined by bit pattern in bbb
: rbs :a c@ or ac! ;

( bbb reg -- )
\ tools
\ clear the bits of reg defined by bit pattern in bbb
: rbc >a not ac@ and ac! ;

( reg -- )
\ tools
\ read register/ram byte contents and print in binary form
: rb? c@ bin <# # # # # # # # # #> type space decimal ;

( reg -- )
\ tools
\ read register/ram byte contents and print in hex form
: r? c@ .$ ;

\ setup fence which is the lowest address that we can forget words
find r? val fence

( c: name -- )
: forget
  find            ( nfa )
  ?dup
  if
    \ nfa must be greater than fence
    dup           ( nfa nfa)
    fence         ( nfa nfa fence )
    >             ( nfa nfa>fence )
    if
      \ nfa is valid
      \ set dp to nfa
      dup           ( nfa nfa )
      to dp         ( nfa )
      \ set wid to lfa
      nfa>lfa       ( lfa )
      @i            ( nfa )
      wid           ( nfa wid )
      !e
    else
      drop  
    then
  then
;

find forget to fence
