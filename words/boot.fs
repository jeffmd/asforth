dp
pname header dup $FF00 or (s,)
  current @ @e , 
  smudge !
  dp latest !
  ffrst 1 state !
    dp >r >r dup $FF00 or (s,) r> @e , r>
  [
  ;opt uwid

pname (create) current @ header
  smudge !
  dp latest !
  ffrst 1 state !
    pname current @ header
  [
  ;opt uwid

(create) ] 
  smudge !
  dp latest !
  ffrst 1 state !
    ffrst 1 state !
  [
  ;opt uwid

(create) :
  smudge !
  dp latest !
  ]
    (create) smudge ! dp latest ! ]
  [
  ;opt uwid


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
    stib
    nip
    >in
    !
[ ;opt uwid immediate

\ boot.fs - bootstrap the forth compiler
\ header, (create), and ] are created manually
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
