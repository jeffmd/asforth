
; ( --  )  
; Interrupt
; turns on all interrupts
forthword_ INTON, INLINE_OPT, 4, "int+"
    sei
    ret

; ( -- ) 
; Interrupt
; turns off all interrupts
forthword_ INTOFF, INLINE_OPT, 4, "int-"
    cli
    ret


; ( --  )  
; Interrupt
; re-enables interrupts in an ISR
forthword_ ISREND, INLINE_OPT, 4, "reti"
    reti
    ret ; needed for inline marker
