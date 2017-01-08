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

\ get a valid wid from the context
\ tries to get the top vocabulary
\ if no valid entries then defaults to Forth wid
: wid@ ( -- wid )
  context@
  ?if else drop context @ then
;

\ wordlist record fields:
\ [0] word:dcell: address to nfa of most recent word
\     added to this wordlist
\ [1] Name:dcell: address to nfa of vocabulary name 
\ [2] link:dcell: address to previous sibling wordlist to
\     form vocabulary linked list
\ [3] child:dcell: address to head of child wordlist

\ add link field offset
: wid:link ( wid -- wid:link) dcell+ dcell+ ;
\ add child field offset
: wid:child ( wid -- wid:child ) 3 dcell* + ;

\ initialize wid fields of definitions vocabulary
: widinit ( wid -- wid )
  \ wid.word = 0
  0 over !e ( wid )

  \ parent wid child field is in cur@->child
  dup cur@ wid:child  ( wid wid parentwid.child )
  2over ( wid wid parentwid.child wid parentwid.child )
  @e   ( wid wid parentwid.child wid childLink )
  swap wid:link ( wid wid parentwid.child childLink wid.link )

  \ wid.child = 0
  0 over dcell+ !e ( wid wid parentwid.child childLink wid.link )
  \ wid.link = childLink
  !e ( wid wid parentwid.child )
  \ parentwid.child = wid
  !e ( wid )
;

: wordlist ( -- wid )
  \ get head address in eeprom for wid
  edp      ( wid )
  \ allocate  4 16bit words in eeprom
  dup 4 dcell* + ( wid edp+8 )
  to edp       ( wid )
  widinit  ( wid )
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
    2drop [compile] only
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
  wordlist dup , ( wid )
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
cur@ @e ( nfa )
\ get the forth wid, initialize it and set name field
\ forthwid.word is already initialized
context @ dcell+ ( nfa forthwid.name )
\ write forth nfa to name field
\ forthwid.name = nfa
tuck !e ( forthwid.name )
\ forthwid.link = 0
dcell+ 0 over !e ( forthwid.link )
\ forthwid.child = 0
dcell+ 0 swap !e ( )


\ print name field
: .nf ( nfa -- )
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
      .nf                    ( cnt addr )
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
    wid@
    @e                       ( 0 addr )
    lwords
;

\ list the root words
: rwords ( -- )
  [ find WIPE lit ]
  lwords
;

\ list active vocabularies
: order ( -- )
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
      .nf
    then
    \ decrement index
    1-
  repeat
  drop
  ." Forth Root" cr
  ." definitions: "
  cur@ dcell+ @e .nf cr
;

\ print child vocabularies
: .childvocs ( spaces wid -- )
  begin
  \ while link is not zero
  ?while  ( spaces linkwid )
    \ print indent
    over spaces ." |- "
    \ get name from name field
    dcell+ dup @e ( spaces linkwid.name name )
    \ print name and line feed
    .nf cr ( spaces link.name )
    \ get link field
    dcell+ ( spaces linkwid.link )
    \ increase spaces for indenting child vocabularies
    over 2+ over ( spaces linkwid.link spaces+2 linkwid.link )
    \ get child link and recurse: print child vocabularies
    dcell+ @e recurse ( spaces linkwid.link )
    \ get link for next sibling
    @e
  repeat
  2drop
;

\ list context vocabulary and all child vocabularies
\ order is newest to oldest
: vocs ( -- )
  \ start spaces at 2
  2
  \ get top search vocabulary address
  \ it is the head of the vocabulary linked list
  wid@  ( wid )
  \ print context vocabulary
  dup dcell+ @e .nf cr
  \ get child link of linked list
  wid:child @e ( linkwid )
  .childvocs cr
;
