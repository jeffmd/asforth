\ core.fs

\ ( "ccc<paren>" -- )
\ Compiler
\ skip everything up to the closing bracket on the same line
: (
    $29 parse
    2drop
; immediate


( -- )
\ make most current word compile only
: :c
    $F7FF widf
; immediate

( -- )
\ make most current word inlinned
: inlined
    $FEFF widf
; immediate

( -- )
\ make most current word immediate and compile only
: :ic
    $77FF widf
; immediate

\ make most current word call only
: call
  $FBFF widf
; immediate

( -- ) ( C: x "<spaces>name" -- )
\ create a dictionary entry and register in word list
: rword
    (create)      ( voc-link )
    cur@          ( voc-link wid )
    !e            ( )
;

\ search dictionary for name, returns XT or 0
: 'f  ( "<spaces>name" -- XT XTflags )
    pname 
    findw
    nfa>xtf
;

( -- ) ( C: "<space>name" -- )
\ Compiler
\ what 'f does in the interpreter mode, do in colon definitions
\ and xt and flag are compiled as two literals
: ['f]
    'f
    swap
    lit
    lit
; :ic

\ search dictionary for name, returns XT
: '  ( "<spaces>name" -- XT )
    'f
    drop
;

( -- ) ( C: "<space>name" -- )
\ Compiler
\ what ' does in the interpreter mode, do in colon definitions
\ compiles xt as literal
: [']
    '
    lit
; :ic


( C:"<spaces>name" -- 0 | nfa )
\ Dictionary
\ search dictionary for name, returns nfa if found or 0 if not found
: find
    pname findw
;


