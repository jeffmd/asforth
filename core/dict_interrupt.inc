
; ( --  )  
; Interrupt
; turns on all interrupts
forthword_ INTON, 0x8004, "+int"
    sei
    ret

; ( -- ) 
; Interrupt
; turns off all interrupts
forthword_ INTOFF, 0x8004, "-int"
    cli
    ret


; ( --  )  
; Interrupt
; re-enables interrupts in an ISR
forthword_ ISREND, 0x8004, "reti"
isr:
    reti

