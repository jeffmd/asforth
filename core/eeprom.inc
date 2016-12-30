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
    addr_ 0      ; vocabulary sibling link
    addr_ 0		 ; child vocabulary link

; default user area
EE_INITUSER:
    .word 0  ; USER_STATE
    .word 0  ; USER_PAUSE
    
    .word 0  ; USER_HANDLER
    .word 10 ; USER_BASE
    
    addr_ XT_TX  ; USER_EMIT
    addr_ XT_RX  ; USER_KEY
    .word 1      ; USER_G_IN
    addr_ REFILLTIB ; USER_REFILL  
