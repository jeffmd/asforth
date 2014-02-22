
; ( --  )  
; Interrupt
; turns on all interrupts
forthword_ INTON, 0x8104, "int+"
    sei
    ret

; ( -- ) 
; Interrupt
; turns off all interrupts
forthword_ INTOFF, 0x8104, "int-"
    cli
    ret


; ( --  )  
; Interrupt
; re-enables interrupts in an ISR
forthword_ ISREND, 0x8104, "reti"
isr:
    reti
