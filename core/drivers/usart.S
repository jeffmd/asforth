; ( -- )
; MCU
; initialize usart
forthword_ USART, 0, 6, "+usart"

  doliteral_ USART_B_VALUE
  doliteral_ USART_B
  call CSTORE

  doliteral_ USART_C_VALUE
  doliteral_ USART_C | bm_USARTC_en
  call CSTORE

  rcall UBRR
  dup_
  call BYTESWAP
  doliteral_ BAUDRATE_HIGH
  call CSTORE
  doliteral_ BAUDRATE_LOW
  call CSTORE

  call XT_USART_INIT_RX
  jmp XT_USART_INIT_TX
