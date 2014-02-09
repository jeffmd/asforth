( -- padsize ) 
\ Environment
\ Size of the PAD buffer in bytes
: /pad
    sp@
    pad
    -
;

( -- hldsize ) 
\ Environment
\ size of the pictured numeric output buffer in bytes

: /hold
    pad
    here
    -
;
