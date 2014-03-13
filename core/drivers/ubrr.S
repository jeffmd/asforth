; ( -- v) 
; MCU
; returns usart UBRR settings
forthword_ UBRR, 0, 4, "ubrr"
  call DOVALUE
  .word EE_UBRRVAL
  addr_ FETCHE
  addr_ STOREE
