pname (create) current @ header
smudge ! dp latest ! ]
  pname current @ header
[ ;opt uwid

(create) :
smudge ! dp latest ! ]
  (create) smudge ! dp latest ! ]
[ ;opt uwid


: cur@
  current @
[ ;opt uwid

: widf 
    cur@
    @e
    dup
    @i
    rot and
    swap
    !i
[ ;opt uwid

: immediate
    $7FFF widf
[ ;opt uwid immediate

: \
    source
    nip
    >in
    !
[ ;opt uwid immediate

\ boot.fs - bootstrap the forth compiler
\ (create) is created manually
\ use (create) to make : then define the rest manually
\ : can now be used to define a new word but must manually 
\ terminate the definition of a new word

\ define ; which is used when finishing the compiling of a word
: ;
  \ change to interpret mode and override to compile [
  [ pname [ findw nfa>xtf cxt ]
  \ back in compile mode
  ;opt uwid
[ ;opt uwid immediate
