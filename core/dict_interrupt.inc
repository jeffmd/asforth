
; ( --  )  
; Interrupt
; turns on all interrupts
forthword_ INTON, 0, 4, "int+"
    sei
    ret

; ( -- ) 
; Interrupt
; turns off all interrupts
forthword_ INTOFF, 0, 4, "int-"
    cli
    ret


; ( --  )  
; Interrupt
; re-enables interrupts in an ISR
forthword_ ISREND, 0, 4, "reti"
isr:
    reti
