; ISR routines
;.pushsection .data
;intcur: .space 1
;intvec: .space INTVECTORS * CELLSIZE
;.popsection

; interrupt routine gets called (again) by rcall! This gives the
; address of the int-vector on the stack.
;isr:
;    st -Y, r0
;    in r0, SREG
;    st -Y, r0
;    pop r0
;    pop r0          ; = intnum * intvectorsize + 1 (address following the rcall)
;    dec r0
;.if intvecsize == 1 ;
;    lsl r0
;.endif
;    sts intcur, r0
;    ld r0, Y+
;    out SREG, r0
;    ld r0, Y+
;    set ; set the interrupt flag for the inner interpreter
;    ret ; returns the interrupt, the rcall stack frame is removed!
