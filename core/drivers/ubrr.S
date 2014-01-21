; ( -- v) 
; MCU
; returns usart UBRR settings
forthword_ UBRR, 0xff04, "ubrr"
  call DOVALUE
  .word EE_UBRRVAL
  addr_ FETCHE
  addr_ STOREE
