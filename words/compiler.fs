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
