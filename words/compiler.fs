: immediate
    wid
    @e
    dup
    @i
    $7FFF and
    swap
    !i
;

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

: [compile]
  ' 0 cxt
; immediate

: compile
  ['] docompile $0400 cxt
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
