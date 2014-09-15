\ vocabulary.fs - words for managing the words

\ get context array address using context index
: context# ( -- addr )
  context dup 1- c@ 2* +
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
  dup 2+ to edp \ allocate a word in eeprom
;

: also ( -- )
  context@
  \ increment index
  context 1- 1+c!
  context!
  
;


: previous ( -- )
  \ get current index and decrement by 1
  context 1- dup c@ 1- dup
  \ index must be >= 1
  0> if
       0 context! swap c!
     else
       ddrop
     then
;

\ Used in the form:
\ cccc DEFINITIONS
\ Set the CURRENT vocabulary to the CONTEXT vocabulary. In the
\ example, executing vocabulary name cccc made it the CONTEXT
\ vocabulary and executing DEFINITIONS made both specify vocabulary
\ cccc.

: definitions
    context@
    current !
;

\ A defining word used in the form:
\     vocabulary cccc  
\ to create a vocabulary definition cccc. Subsequent use of cccc will
\ make it the CONTEXT vocabulary which is searched first by INTERPRET.
\ The sequence "cccc DEFINITIONS" will also make cccc the CURRENT
\ vocabulary into which new definitions are placed.

\ In asforth, cccc will be so chained as to include all definitions
\ of the vocabulary in which cccc is itself defined. All vocabularys
\ ultimately chain to Forth. By convention, vocabulary names are to be
\ declared IMMEDIATE. See VOC-LINK.

: vocabulary ( -- ) ( C:cccc )
\ figforth original
\ <builds
\ 0A081H lit ,
\ current @ cfa , \ 
\ here voc-link @ ,
\ voc-link !
\ does> 2+ context ! ;
  create
  [compile] immediate
  \ allocate space in eeprom for head and tail of vocab word list
  wordlist ,

  does>
   @i \ get eeprom header address
   context!
;



\ List the names of the definitions in the context vocabulary.
\ Does not list other linked vocabularies.
\ Use words to see all words in the top context search.
: words
    0                      ( 0 )
    context@
    ?if else drop context @ then
    @e                       ( 0 addr )
    begin
      ?dup                   ( cnt addr addr )
    while                    ( cnt addr ) \ is nfa = counted string
      dup                    ( cnt addr addr )
      $l $FF and             ( cnt addr addr n ) \ mask immediate bit
      itype space            ( cnt addr )
      nfa>lfa                ( cnt lfa )
      @i                     ( cnt addr )
      swap                   ( addr cnt )
      1+                     ( addr cnt+1 )
      swap                   ( cnt+1 addr )
    repeat 

    cr .
;
