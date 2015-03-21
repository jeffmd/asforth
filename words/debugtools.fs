

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


( addr1 cnt -- addr2)
: dmp
 over .$ [char] : emit space
 begin
   ?while icell- swap dup @i .$ icell+ swap
 repeat
 drop
;


( addr -- )
\ Tools
\ print the contents at ram word addr
: ? @ . ;

\ print the contents at ram char addr
: c? c@ . ;

( bbb reg -- )
\ tools
\ set the bits of reg defined by bit pattern in bbb
: rbs :a c@ or ac! ;

( bbb reg -- )
\ tools
\ clear the bits of reg defined by bit pattern in bbb
: rbc >a not ac@ and ac! ;

\ modify bits of reg defined by mask
: rbm ( val mask reg -- )
    >a ac@ and or
    ac!
;


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
  pname
  cur@
  findnfa            ( nfa )
  ?dup
  if
    \ nfa must be greater than fence
    dup           ( nfa nfa)
    fence         ( nfa nfa fence )
    $3800
    within             ( nfa nfa>fence )
    if
      \ nfa is valid
      \ set dp to nfa
      dup           ( nfa nfa )
      dp! dp!e      ( nfa )
      \ set context wid to lfa
      nfa>lfa       ( lfa )
      @i            ( nfa )
      cur@     ( nfa wid )
      !e            (  )
    else
      drop  
    then
  then
;

find forget to fence

\ create a marker word
\ when executed it will restore dp, here and current
\ back to when marker was created
: marker  ( c: name -- )
  \ copy current word list, current wid, dp, here
  cur@ dup @e dp here
  create
  \ save here, dp, current wid, current word list
  , , , ,
  does> ( addr )
    \ restore here
    dup @i to here
    \ restore dp
    icell+ dup @i dp! dp!e 
    \ restore current wid
    icell+ dup @i 
    swap icell+ @i !e
    \ only Forth and Root are safe vocabs
    [compile] only
;
