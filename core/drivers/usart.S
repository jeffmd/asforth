; calculate baud rate error
.equ UBRR_VAL,   ((F_CPU+BAUD*8)/(BAUD*16)-1)  ; smart round
.equ BAUD_REAL,  (F_CPU/(16*(UBRR_VAL+1)))     ; effective baud rate
.equ BAUD_ERROR, ((BAUD_REAL*1000)/BAUD-1000)  ; error in pro mille

.if ((BAUD_ERROR>10) || (BAUD_ERROR<-10))       ; accept +/-10 error (pro mille)
  .error "Serial line cannot be set up properly (systematic baud error too high)"
.endif


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

  doliteral_ UBRR_VAL
  doliteral_ BAUDRATE_HIGH
  call HSTORE

  call XT_USART_INIT_RX
  jmp XT_USART_INIT_TX
