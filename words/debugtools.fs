
 -- n )
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

: .hex4 hex <# # # # # #> type space decimal ;

( addr1 cnt -- addr2)
: dmp
 over .hex4 [char] : emit space
 begin
   ?while 1- swap dup @i .hex4 1+ swap
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
