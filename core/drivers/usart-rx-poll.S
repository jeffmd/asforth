; ( -- c)
; MCU
; wait for one character and read it from the terminal connection using register poll
forthword_ RX_POLL, 0, "rx-poll"
  ; wait for data ready
  rcall RXQ_POLL
  zerosense_ 
  breq RX_nodata
  ; get character from usart
  doliteral_ USART_DATA
  jmp CFETCH
  
RX_nodata:
  jmp ZERO

; ( -- f)
; MCU
; check if a character has been received by usart using register poll
forthword_ RXQ_POLL, 0, "rx?-poll"
  ;rcall PAUSE
  doliteral_ bm_USART_RXRD
  dup_
  doliteral_ USART_A
  call CFETCH
  call AND
  jmp EQUAL
