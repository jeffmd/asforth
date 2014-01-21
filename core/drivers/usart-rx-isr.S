; ( -- c)
; MCU
; get 1 character from input queue, wait if needed using interrupt driver
forthword_ RX_ISR:
    .dw $ff06
    .db "rx-isr"
    .dw forthword_ HEAD
    .set forthword_ HEAD = forthword_ RX_ISR
XT_RX_ISR:
    .dw DO_COLON
PFA_RX_ISR:
  .dw XT_RXQ_ISR
  .dw XT_DOCONDBRANCH
  .dw PFA_RX_ISR
  .dw XT_DOLITERAL
  .dw usart_rx_out
  .dw XT_CFETCH
  .dw XT_DUP
  .dw XT_1PLUS
  .dw XT_DOLITERAL
  .dw usart_rx_mask
  .dw XT_AND
  .dw XT_DOLITERAL
  .dw usart_rx_out
  .dw XT_CSTORE
  .dw XT_DOLITERAL
  .dw usart_rx_data
  .dw XT_PLUS
  .dw XT_CFETCH
  .dw XT_EXIT

; ( -- f)  
; MCU
; check if unread characters are in the input queue using interrupt driver
forthword_ RXQ_ISR:
    .dw $ff07
    .db "rx?-isr",0
    .dw forthword_ HEAD
    .set forthword_ HEAD = forthword_ RXQ_ISR
XT_RXQ_ISR:
    .dw DO_COLON
PFA_RXQ_ISR:
  .dw XT_PAUSE
  .dw XT_DOLITERAL
  .dw usart_rx_out
  .dw XT_CFETCH
  .dw XT_DOLITERAL
  .dw usart_rx_in
  .dw XT_CFETCH
  .dw XT_NOTEQUAL
  .dw XT_EXIT
