;eeprom.inc

    .word -1           ; EEPROM Address 0 should not be used
EE_DP:
    addr_ DPSTART      ; Dictionary Pointer in flash
EE_HERE:
    .word HERESTART    ; RAM Memory Allocation pointer
EE_EDP:
    .word EDPSTART     ; EEProm Memory Allocation pointer
EE_TURNKEY:
    addr_ NOOP         ; TURNKEY


EE_FORTHWORDLIST:
    addr_ 0      ; pre-defined (compiled in) top of root forth wordlist
    addr_ 0      ; VE_FORTH pointer

; default user area
EE_INITUSER:
    .word 0  ; USER_STATE
    .word 0  ; USER_PAUSE
    .word rstackstart  ; USER_RP
    .word stackstart   ; USER_SP0
    .word stackstart   ; USER_SP
    
    .word 0  ; USER_HANDLER
    .word 10 ; USER_BASE
    
    addr_ XT_TX  ; USER_EMIT
    addr_ XT_TXQ ; USER_EMITQ
    addr_ XT_RX  ; USER_KEY
    addr_ XT_RXQ ; USER_KEYQ
    addr_ 0      ; USER_SKEY - not used
    .word 1      ; USER_G_IN
    addr_ REFILLTIB ; USER_REFILL  


;EE_RECOGNIZERS:
;EE_RECOGNIZERLISTLEN:
;    .word 3
;EE_RECOGNIZERLIST:
;    addr_ REC_WORD
;    addr_ REC_NUM
;    addr_ REC_UNK
;    .word -1
;    .word -1
;    .word -1
