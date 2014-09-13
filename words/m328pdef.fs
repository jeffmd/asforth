\ ***** I/O REGISTER DEFINITIONS ******

only forth definitions
vocabulary I/O
also I/O definitions

\ USART0
$C6 con UDR0	\ USART I/O Data Register
$C0 con UCSR0A	\ USART Control and Status Register A
$C1 con UCSR0B	\ USART Control and Status Register B
$C2 con UCSR0C	\ USART Control and Status Register C
$C4 con UBRR0	\ USART Baud Rate Register  Bytes

\ TWI
$BD con TWAMR	\ TWI (Slave) Address Mask Register
$B8 con TWBR	\ TWI Bit Rate register
$BC con TWCR	\ TWI Control Register
$B9 con TWSR	\ TWI Status Register
$BB con TWDR	\ TWI Data register
$BA con TWAR	\ TWI (Slave) Address register

\ TIMER_COUNTER_0
$48 con OCR0B	\ Timer/Counter0 Output Compare Register
$47 con OCR0A	\ Timer/Counter0 Output Compare Register
$46 con TCNT0	\ Timer/Counter0
$45 con TCCR0B	\ Timer/Counter Control Register B
$44 con TCCR0A	\ Timer/Counter  Control Register A
$6E con TIMSK0	\ Timer/Counter0 Interrupt Mask Register
$35 con TIFR0	\ Timer/Counter0 Interrupt Flag register

\ TIMER_COUNTER_1
$6F con TIMSK1	\ Timer/Counter Interrupt Mask Register
$36 con TIFR1	\ Timer/Counter Interrupt Flag register
$80 con TCCR1A	\ Timer/Counter1 Control Register A
$81 con TCCR1B	\ Timer/Counter1 Control Register B
$82 con TCCR1C	\ Timer/Counter1 Control Register C
$85 con TCNT1H	\ Timer/Counter1  Bytes
$89 con OCR1AH	\ Timer/Counter1 Output Compare Register  Bytes
$8B con OCR1BH	\ Timer/Counter1 Output Compare Register  Bytes
$87 con ICR1H	\ Timer/Counter1 Input Capture Register  Bytes
$43 con GTCCR	\ General Timer/Counter Control Register

\ TIMER_COUNTER_2
$70 con TIMSK2	\ Timer/Counter Interrupt Mask register
$37 con TIFR2	\ Timer/Counter Interrupt Flag Register
$B0 con TCCR2A	\ Timer/Counter2 Control Register A
$B1 con TCCR2B	\ Timer/Counter2 Control Register B
$B2 con TCNT2	\ Timer/Counter2
$B4 con OCR2B	\ Timer/Counter2 Output Compare Register B
$B3 con OCR2A	\ Timer/Counter2 Output Compare Register A
$B6 con ASSR	\ Asynchronous Status Register

\ AD_CONVERTER
$7C con ADMUX	\ The ADC multiplexer Selection Register
$78 con ADC	    \ ADC Data Register  Bytes
$7A con ADCSRA	\ The ADC Control and Status register A
$7B con ADCSRB	\ The ADC Control and Status register B
$7E con DIDR0	\ Digital Input Disable Register

\ ANALOG_COMPARATOR
$50 con ACSR	\ Analog Comparator Control And Status Register
$7F con DIDR1	\ Digital Input Disable Register 1

\ PORTB
$25 con PORTB	\ Port B Data Register
$24 con DDRB	\ Port B Data Direction Register
$23 con PINB	\ Port B Input Pins

\ PORTC
$28 con PORTC	\ Port C Data Register
$27 con DDRC	\ Port C Data Direction Register
$26 con PINC	\ Port C Input Pins

\ PORTD
$2B con PORTD	\ Port D Data Register
$2A con DDRD	\ Port D Data Direction Register
$29 con PIND	\ Port D Input Pins

\ EXTERNAL_INTERRUPT
$69 con EICRA	\ External Interrupt Control Register
$3D con EIMSK	\ External Interrupt Mask Register
$3C con EIFR	\ External Interrupt Flag Register
$68 con PCICR	\ Pin Change Interrupt Control Register
$6D con PCMSK2	\ Pin Change Mask Register 2
$6C con PCMSK1	\ Pin Change Mask Register 1
$6B con PCMSK0	\ Pin Change Mask Register 0
$3B con PCIFR	\ Pin Change Interrupt Flag Register

\ SPI
$4E con SPDR	\ SPI Data Register
$4D con SPSR	\ SPI Status Register
$4C con SPCR	\ SPI Control Register

\ WATCHDOG
$60 con WDTCSR	\ Watchdog Timer Control Register

\ CPU
$64 con PRR	    \ Power Reduction Register
$66 con OSCCAL	\ Oscillator Calibration Value
$61 con CLKPR	\ Clock Prescale Register
$5F con SREG	\ Status Register
$5D con SP	    \ Stack Pointer 
$57 con SPMCSR	\ Store Program Memory Control and Status Register
$55 con MCUCR	\ MCU Control Register
$54 con MCUSR	\ MCU Status Register
$53 con SMCR	\ Sleep Mode Control Register
$4B con GPIOR2	\ General Purpose I/O Register 2
$4A con GPIOR1	\ General Purpose I/O Register 1
$3E con GPIOR0	\ General Purpose I/O Register 0

\ EEPROM
$41 con EEAR	\ EEPROM Address Register  Bytes
$40 con EEDR	\ EEPROM Data Register
$3F con EECR	\ EEPROM Control Register

\ Interrupts
&02  con INT0 \ External Interrupt Request 0
&04  con INT1 \ External Interrupt Request 1
&06  con PCI0 \ Pin Change Interrupt Request 0
&08  con PCI1 \ Pin Change Interrupt Request 0
&10  con PCI2 \ Pin Change Interrupt Request 1
&12  con WDT  \ Watchdog Time-out Interrupt
&14  con CM2A \ Timer/Counter2 Compare Match A
&16  con CM2B \ Timer/Counter2 Compare Match A
&18  con OVF2 \ Timer/Counter2 Overflow
&20  con ICP1 \ Timer/Counter1 Capture Event
&22  con CM1A \ Timer/Counter1 Compare Match A
&24  con CM1B \ Timer/Counter1 Compare Match B
&26  con OVF1 \ Timer/Counter1 Overflow
&28  con CM0A \ TimerCounter0 Compare Match A
&30  con CM0B \ TimerCounter0 Compare Match B
&32  con OVF0 \ Timer/Counter0 Overflow
&34  con SPI  \ SPI Serial Transfer Complete
&36  con URXC \ USART Rx Complete
&38  con UDRE \ USART, Data Register Empty
&40  con UTXC \ USART Tx Complete
&42  con ADCC  \ ADC Conversion Complete
&44  con ERDY \ EEPROM Ready
&46  con ACI  \ Analog Comparator
&48  con TWI  \ Two-wire Serial Interface
&50  con SPM  \ Store Program Memory Read

\ turn port into direct i/o address
: DIO ( port -- )
  $20 - 
;
