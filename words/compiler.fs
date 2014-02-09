: immediate
    wid
    @e
    dup
    @i
    $7FFF and
    swap
    !i
; immediate

: \
    source
    nip
    >in
    !
; immediate

\ ( "ccc<paren>" -- )
\ Compiler
\ skip everything up to the closing bracket on the same line
: (
    $29
    parse
    2drop
; immediate

\ force compile any word including immediate words
: [compile]
  'f cxt
; immediate

( -- ) ( C: "<space>name" -- )
\ Compiler
\ what ' does in the interpreter mode, do in colon definitions
\ compiles xt as literal
: [']
    '
    [compile] lit
; immediate


( -- ) ( C: "<space>name" -- )
\ Compiler
\ what 'f does in the interpreter mode, do in colon definitions
\ and xt and flag are compiled as two literals
: ['f]
    'f
    swap
    [compile] lit
    [compile] lit
; immediate


: compile
  ['f] docompile cxt
  ' ,
; immediate

( x -- ) ( C: x "<spaces>name" -- )
\ Compiler
\ create a constant in the dictionary
: con
    rword
    [compile] lit  
    [compile] ret,
;

( cchar -- ) 
\ Compiler
\ create a dictionary entry for a variable and allocate 1 cell RAM
: var
    here
    con
    2
    allot
;

( -- a-addr ) ( C: "<spaces>name" -- )
\ Dictionary
\ create a dictionary header. XT is (constant),
\ with the address of the data field of name
: create
    rword
    \ leave address after call on tos
    compile popret
;


( -- )
\ Compiler
\ replace the XT written by CREATE to call the code that follows does>
\ does not return to caller
: (does>)
    \ change call at XT to code after (does>)
    \ get current word and then get its XT being compiled
    \ code at XT is 'call POPRET'
    \ want to change POPRET address to return address
    r>
    wid
    @e
    nfa>lfa
    2+         \ lfa>xt+1
    !i
;

( i*x -- j*y ) ( R: nest-sys1 -- ) ( C: colon-sys1 -- colon-sys2 )
\ Compiler
\ organize the XT replacement to call other colon code
: does>
    \ compile pop return to tos which is used as 'THIS' pointer
    compile (does>)
    compile r>
; immediate
