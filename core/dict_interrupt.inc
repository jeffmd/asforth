
; ( --  )  
; Interrupt
; turns on all interrupts
forthword_ INTON, INLINE_OPT, "int+"
    sei
    ret

; ( -- ) 
; Interrupt
; turns off all interrupts
forthword_ INTOFF, INLINE_OPT, "int-"
    cli
    ret


; ( --  )  
; Interrupt
; re-enables interrupts in an ISR
forthword_ ISREND, INLINE_OPT, "reti"
    reti
    ret ; needed for inline marker

; ( -- )
; ISR
; define the start of an Interrupt Service Routine
; must be called
forthword_ ISTART, DIS_CALL_OPT, "(i:)"
    ; save important registers
    movw isrZL, ZL
    movw isrXL, XL
    movw isrt0, temp0
    movw isrt2, temp2
    
    ; pop return address
    pop ZH
    pop ZL
    
    ; save status register
    in temp0, SREG
    push temp0
    ijmp

; ( -- )
; ISR
; define the end of an Interupt Service Routine
forthword_ IEND, 0, "(i;)"
    movw ZL, isrZL
    movw XL, isrXL
    movw temp0, isrt0
    movw temp2, isrt2
    
    pop isrt0
    out SREG, isrt0    
    reti
