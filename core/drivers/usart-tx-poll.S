; (c -- )
; MCU
; check availability and send one character to the terminal using register poll
forthword_ TX_POLL, 0, "tx-poll"
  ; wait for data ready
  rcall TXQ_POLL
  zerosense_
  breq TX_POLL
  ; send to usart
  doliteral_ USART_DATA
  jmp CSTORE

; ( -- f) MCU
; MCU
; check if a character can be send using register poll
forthword_ TXQ_POLL, 0, "tx?-poll"
  ;rcall PAUSE
  doliteral_ USART_A
  call CFETCH
  doliteral_ bm_USART_TXRD
  jmp AND
