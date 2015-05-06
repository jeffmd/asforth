\ vocabulary.fs - words for managing the words

\ get context index address
: contidx ( -- addr )
  context 1-
;

\ get context array address using context index
: context# ( -- addr )
  context contidx c@ dcell* +
;

\ get a wordlist id from context array
: context@ ( -- wid )
  context# @
;

\ save wordlist id in context array at context index
: context! ( wid -- )
  context# !
;


: wordlist ( -- wid )
  edp 0 over !e \ get head address in eeprom and set to zero
  dup dcell+ dcell+ to edp \ allocate  2 words in eeprom
;

: also ( -- )
  context@
  \ increment index
  contidx 1+c!
  context!
  
; immediate


: previous ( -- )
  \ get current index and decrement by 1
  contidx dup c@ 1- dup
  \ index must be >= 1
  0>
  if
    0 context! swap c!
  else
    2drop
  then
; immediate

\ Used in the form:
\ cccc DEFINITIONS
\ Set the CURRENT vocabulary to the CONTEXT vocabulary. In the
\ example, executing vocabulary name cccc made it the CONTEXT
\ vocabulary and executing DEFINITIONS made both specify vocabulary
\ cccc.

: definitions
    context@
    ?if current ! then
; immediate

\ A defining word used in the form:
\     vocabulary cccc  
\ to create a vocabulary definition cccc. Subsequent use of cccc will
\ make it the CONTEXT vocabulary which is searched first by INTERPRET.
\ The sequence "cccc DEFINITIONS" will also make cccc the CURRENT
\ vocabulary into which new definitions are placed.

\ By convention, vocabulary names are automaticaly declared IMMEDIATE.

: vocabulary ( -- ) ( C:cccc )
  create
  [compile] immediate
  \ allocate space in eeprom for head and tail of vocab word list
  wordlist dup ,
  \ get nfa and store in second field of wordlist record in eeprom
  cur@ @e swap dcell+ !e
  does>
   @i \ get eeprom header address
   context!
;

\ Set context to Forth vocabulary
: Forth ( -- )
  context @ context!
; immediate

\ setup forth name pointer in forth wid name field
\ get forth nfa - its the most recent word created
cur@ @e
\ get the forth wid and adjust to name field 
context @ dcell+
\ write forth nfa to name field
!e 

\ print name field
: ?nf ( nfa -- )
      $l $FF and             ( cnt addr addr n ) \ mask immediate bit
      itype space            ( cnt addr )
;
 
\ list words starting at a name field address
: lwords ( nfa -- )
    0 swap
    begin
      ?dup                   ( cnt addr addr )
    while                    ( cnt addr ) \ is nfa = counted string
      dup                    ( cnt addr addr )
      ?nf                    ( cnt addr )
      nfa>lfa                ( cnt lfa )
      @i                     ( cnt addr )
      swap                   ( addr cnt )
      1+                     ( addr cnt+1 )
      swap                   ( cnt+1 addr )
    repeat 

    cr ." count: " .
;

\ List the names of the definitions in the context vocabulary.
\ Does not list other linked vocabularies.
\ Use words to see all words in the top context search.
: words ( -- )
    context@
    ?if else drop context @ then
    @e                       ( 0 addr )
    lwords
;

\ list the root words
: rwords ( -- )
  [ find WIPE lit ]
  lwords
;

\ list active vocabularies
: vocabs ( -- )
  ." Search: "
  \ get context index and use as counter
  contidx c@
  begin
  \ iterate through vocab array and print out vocab names
  ?while
    dup dcell* context +
    \ get context wid
    @
    \ if not zero then print vocab name 
    ?dup if
      \ next cell in eeprom has name field address 
      dcell+ @e
      ?nf
    then
    \ decrement index
    1-
  repeat
  drop
  ." Forth Root" cr
  ." definitions: "
  cur@ dcell+ @e ?nf cr
;
