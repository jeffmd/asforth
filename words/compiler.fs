\ compiler.fs 

\ force compile any word including immediate words
: [compile]
  'f cxt
; :ic


\ read the following cell from the executing word and append it
\ to the current dictionary position.
\ must use call/rcall

: (compile)  ( -- )
    r>r+     ( raddr ) ( R: raddr+1 )
    @i       ( nfa )
    nfa>xtf  ( xt xtflags )
    cxt
; call

\ compile into pending new word
: compile ( C: x "<spaces>name" -- )
  ['f] (compile) cxt
  find ,
; :ic


( -- a-addr ) ( C: "<spaces>name" -- )
\ Dictionary
\ create a dictionary header that will push the address of the
\ data field of name.
\ is used in conjunction with does>
: create
    rword
    \ leave address after call on tos
    compile popret
;


\ copy the first character of the next word onto the stack
: char  ( "<spaces>name" -- c )
    pname
    drop
    c@
;

( -- c ) ( C: "<space>name" -- )
\ skip leading space delimites, place the first character
\ of the word on the stack
: [char]
    char
    lit
; immediate

( -- )
\ Compiler
\ replace the instruction written by CREATE to call the code that follows does>
\ does not return to caller
: (does>)
    \ change call at XT to code after (does>)
    \ get current word and then get its XT being compiled
    \ code at XT is 'call POPRET'
    \ want to change POPRET address to return address
    r>
    cur@ @e
    nfa>lfa
    2+         \ lfa>xt+1
    !i
;

( -- )
\ Compiler
\ organize the XT replacement to call other colon code
\ used in conjunction with create
\ ie: : name create .... does> .... ;
: does>
    \ compile pop return to tos which is used as 'THIS' pointer
    compile (does>)
    compile r>
; :ic

( -- xt )
\ Compiler
\ create an unnamed entry in the dictionary
: :noname
    dp ]
;

( -- start ) 
\ Compiler
\ places current dictionary position for forward
\ branch resolve on TOS and advances DP
: >mark
    dp
    dp+1           \ advance DP
;

( start -- ) 
\ Compiler
\ do forward jump
: >jmp
    ?sp              ( start ) \ check stack integrety
    dp               ( start dest )
    rjmpc            ( )
;

( -- dest ) 
\ Compiler
\ place destination for backward branch
: <mark
    dp            ( dest )
;

( dest -- ) 
\ Compiler
\ do backward jump
: <jmp
    ?sp            \ make sure there is something on the stack
    \ compile a rjmp at current DP that jumps back to mark
    dp             \ ( dest start )
    swap           \ ( start dest )
    rjmpc
    dp+1           \ advance DP
;


\ Compiler
\ compile zerosense and conditional branch forward
: ?brc
    
    compile 0?       \ inline zerosense
    brnz1,
;

\ compile dupzerosense and conditional branch forward
: ??brc
    ?0?,
    brnz1,
;


( f -- ) ( C: -- orig )
\ Compiler
\ start conditional branch
\ part of: if...[else]...then
: if
   ?brc
   >mark 
; :ic

( f -- f ) ( C: -- orig )
\ Compiler
\ start conditional branch, don't consume flag
: ?if
    ??brc
    >mark 
; :ic


( C: orig1 -- orig2 ) 
\ Compiler
\ resolve the forward reference and place
\ a new unresolved forward reference
\ part of: if...else...then
: else
    >mark         \ mark forward rjmp at end of true code
    swap          \ swap new mark with previouse mark
    >jmp          \ rjmp from previous mark to false code starting here
; :ic

( -- ) ( C: orig -- ) 
\ Compiler
\ finish if
\ part of: if...[else]...then
: then
    >jmp
; :ic


( -- ) ( C: -- dest ) 
\ Compiler
\ put the destination address for the backward branch:
\ part of: begin...while...repeat, begin...until, begin...again 
: begin
    <mark
; :ic


( -- ) ( C: dest -- ) 
\ Compiler
\ compile a jump back to dest
\ part of: begin...again

: again
    <jmp
; :ic

( f -- ) ( C: dest -- orig dest ) 
\ Compiler
\ at runtime skip until repeat if non-true
\ part of: begin...while...repeat
: while
    [compile] if
    swap
; :ic

( f -- f) ( C: dest -- orig dest ) 
\ Compiler
\ at runtime skip until repeat if non-true, does not consume flag
\ part of: begin...?while...repeat
: ?while
    [compile] ?if
    swap
; :ic

( --  ) ( C: orig dest -- )
\ Compiler
\ continue execution at dest, resolve orig
\ part of: begin...while...repeat
: repeat
    [compile] again
    >jmp
; :ic


( f -- ) ( C: dest -- ) 
\ Compiler
\ finish begin with conditional branch,
\ leaves the loop if true flag at runtime
\ part of: begin...until
: until
    ?brc
    <jmp
; :ic

( f -- ) ( C: dest -- ) 
\ Compiler
\ finish begin with conditional branch,
\ leaves the loop if true flag at runtime
\ part of: begin...?until
: ?until
    ??brc
    <jmp
; :ic

( -- ) 
\ Compiler
\ compile the XT of the word currently
\ being defined into the dictionary
: recurse
    smudge @ nfa>xtf cxt
; :ic

( n cchar -- ) 
\ Compiler
\ create a dictionary entry for a user variable at offset n
\ : user
\    rword
\    compile douser
\    ,
\ ;

\ store the TOS to the named defer
: to ( n <name> -- )
    '  \ get address of next word from input stream
    state@
    if 
      compile (to)
      ,
    else
      def! \ not in compile state, so do runtime operation
    then

; immediate

\ allocate or release n bytes of memory in RAM
: allot ( n -- )
    here + to here
;

( x -- ) ( C: x "<spaces>name" -- )
\ create a constant in the dictionary
: con
    rword
    lit  
    ret,
;


\ create a dictionary entry for a variable and allocate 1 cell RAM
: var ( cchar -- )
    here
    con
    dcell
    allot
;

( cchar -- ) 
\ Compiler
\ create a dictionary entry for a character variable
\ and allocate 1 byte RAM
: cvar
    here
    con
    1
    allot
;

( n -- )  ( C: x "<spaces>name" -- )
\ Compiler
\ create a dictionary entry for a value and allocate 1 cell in EEPROM.
: val
    rword
    compile (val)
    edp                ( n edp )
    dup                ( n edp edp )
    ,                  ( n edp )
    dup                ( n edp edp )
    dcell+             ( n edp edp+dcell)
    to edp             ( n edp )
    !e                 ( )
    ['] @e ,
    ['] !e ,
;


\ compiles a string from RAM to Flash
: s, ( addr len -- )
    dup
    (s,)
;

( C: addr len -- ) 
\ String
\ compiles a string to flash
: slit
    compile (slit)     ( -- addr n)
    s,
; immediate


( -- addr len) ( C: <cchar> -- ) 
\ Compiler
\ compiles a string to flash,
\ at runtime leaves ( -- flash-addr count) on stack
: s"
    $22
    parse        ( -- addr n)
    state@
    if  \ skip if not in compile mode
      [compile] slit
    then 
; immediate

( -- ) ( C: "ccc<quote>" -- )
\ Compiler
\ compiles string into dictionary to be printed at runtime
: ."
     [compile] s"             \ "
     state@
     if
       compile itype
     else
       type
     then
; immediate
