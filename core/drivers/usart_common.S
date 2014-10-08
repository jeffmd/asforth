.equ USART_C_VALUE , bm_ASYNC | bm_NO_PARITY | bm_1STOPBIT | bm_8BIT
.if WANT_ISR_RX == 1
  .equ USART_B_VALUE , bm_ENABLE_TX | bm_ENABLE_RX | bm_ENABLE_INT_RX
  .include "drivers/usart-isr-rx.S"
  .include "words/usart-rx-isr.S"
  .set XT_RX  , XT_RX_ISR
  .set XT_RXQ , XT_RXQ_ISR
  .set XT_USART_INIT_RX , XT_USART_INIT_RX_ISR
.else
  .equ USART_B_VALUE , bm_ENABLE_TX | bm_ENABLE_RX
  .include "usart-rx-poll.S"
  .set XT_RX  , RX_POLL
  .set XT_RXQ , RXQ_POLL
  .set XT_USART_INIT_RX , NOOP
.endif

.if WANT_ISR_TX == 1
  .include "drivers/usart-isr-tx.S"
  .include "words/usart-tx-isr.S"
  .set XT_TX  , XT_TX_ISR
  .set XT_TXQ , XT_TXQ_ISR
  .set XT_USART_INIT_TX , XT_USART_INIT_TX_ISR
.else
  .include "usart-tx-poll.S"
  .set XT_TX  , TX_POLL
  .set XT_TXQ , TXQ_POLL
  .set XT_USART_INIT_TX , NOOP
.endif

.include "usart.S"
