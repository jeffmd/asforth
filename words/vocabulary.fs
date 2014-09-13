\ vocabulary.fs - words for managing the words


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

: wordlist ( -- wid )
  edp 0 over !e \ get head address in eeprom and set to zero
  dup 2+ to edp \ allocate a word in eeprom
;

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
   >r
   get-order swap drop 
   r> swap set-order

;


\ List the names of the definitions in the context vocabulary. Any key
\ press will terminate the listing.  Does not list other linked
\ vocabularies.  Use words to see all words in the context search.

: list ( -- )

;
