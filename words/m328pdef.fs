\ ***** I/O REGISTER DEFINITIONS ******

\ USART0
&198 con UDR0	\ USART I/O Data Register
&192 con UCSR0A	\ USART Control and Status Register A
&193 con UCSR0B	\ USART Control and Status Register B
&194 con UCSR0C	\ USART Control and Status Register C
&196 con UBRR0	\ USART Baud Rate Register  Bytes
\ TWI
&189 con TWAMR	\ TWI (Slave) Address Mask Register
&184 con TWBR	\ TWI Bit Rate register
&188 con TWCR	\ TWI Control Register
&185 con TWSR	\ TWI Status Register
&187 con TWDR	\ TWI Data register
&186 con TWAR	\ TWI (Slave) Address register
\ TIMER_COUNTER_1
&111 con TIMSK1	\ Timer/Counter Interrupt Mask Register
&54 con TIFR1	\ Timer/Counter Interrupt Flag register
&128 con TCCR1A	\ Timer/Counter1 Control Register A
&129 con TCCR1B	\ Timer/Counter1 Control Register B
&130 con TCCR1C	\ Timer/Counter1 Control Register C
&132 con TCNT1	\ Timer/Counter1  Bytes
&136 con OCR1A	\ Timer/Counter1 Output Compare Register  Bytes
&138 con OCR1B	\ Timer/Counter1 Output Compare Register  Bytes
&134 con ICR1	\ Timer/Counter1 Input Capture Register  Bytes
&67 con GTCCR	\ General Timer/Counter Control Register
\ TIMER_COUNTER_2
&112 con TIMSK2	\ Timer/Counter Interrupt Mask register
&55 con TIFR2	\ Timer/Counter Interrupt Flag Register
&176 con TCCR2A	\ Timer/Counter2 Control Register A
&177 con TCCR2B	\ Timer/Counter2 Control Register B
&178 con TCNT2	\ Timer/Counter2
&180 con OCR2B	\ Timer/Counter2 Output Compare Register B
&179 con OCR2A	\ Timer/Counter2 Output Compare Register A
&182 con ASSR	\ Asynchronous Status Register
\ AD_CONVERTER
&124 con ADMUX	\ The ADC multiplexer Selection Register
&120 con ADC	\ ADC Data Register  Bytes
&122 con ADCSRA	\ The ADC Control and Status register A
&123 con ADCSRB	\ The ADC Control and Status register B
&126 con DIDR0	\ Digital Input Disable Register
\ ANALOG_COMPARATOR
&80 con ACSR	\ Analog Comparator Control And Status Register
&127 con DIDR1	\ Digital Input Disable Register 1
\ PORTB
&37 con PORTB	\ Port B Data Register
&36 con DDRB	\ Port B Data Direction Register
&35 con PINB	\ Port B Input Pins
\ PORTC
&40 con PORTC	\ Port C Data Register
&39 con DDRC	\ Port C Data Direction Register
&38 con PINC	\ Port C Input Pins
\ PORTD
&43 con PORTD	\ Port D Data Register
&42 con DDRD	\ Port D Data Direction Register
&41 con PIND	\ Port D Input Pins
\ TIMER_COUNTER_0
&72 con OCR0B	\ Timer/Counter0 Output Compare Register
&71 con OCR0A	\ Timer/Counter0 Output Compare Register
&70 con TCNT0	\ Timer/Counter0
&69 con TCCR0B	\ Timer/Counter Control Register B
&68 con TCCR0A	\ Timer/Counter  Control Register A
&110 con TIMSK0	\ Timer/Counter0 Interrupt Mask Register
&53 con TIFR0	\ Timer/Counter0 Interrupt Flag register
\ EXTERNAL_INTERRUPT
&105 con EICRA	\ External Interrupt Control Register
&61 con EIMSK	\ External Interrupt Mask Register
&60 con EIFR	\ External Interrupt Flag Register
&104 con PCICR	\ Pin Change Interrupt Control Register
&109 con PCMSK2	\ Pin Change Mask Register 2
&108 con PCMSK1	\ Pin Change Mask Register 1
&107 con PCMSK0	\ Pin Change Mask Register 0
&59 con PCIFR	\ Pin Change Interrupt Flag Register
\ SPI
&78 con SPDR	\ SPI Data Register
&77 con SPSR	\ SPI Status Register
&76 con SPCR	\ SPI Control Register
\ WATCHDOG
&96 con WDTCSR	\ Watchdog Timer Control Register
\ CPU
&100 con PRR	\ Power Reduction Register
&102 con OSCCAL	\ Oscillator Calibration Value
&97 con CLKPR	\ Clock Prescale Register
&95 con SREG	\ Status Register
&93 con SP	    \ Stack Pointer 
&87 con SPMCSR	\ Store Program Memory Control and Status Register
&85 con MCUCR	\ MCU Control Register
&84 con MCUSR	\ MCU Status Register
&83 con SMCR	\ Sleep Mode Control Register
&75 con GPIOR2	\ General Purpose I/O Register 2
&74 con GPIOR1	\ General Purpose I/O Register 1
&62 con GPIOR0	\ General Purpose I/O Register 0
\ EEPROM
&65 con EEAR	\ EEPROM Address Register  Bytes
&64 con EEDR	\ EEPROM Data Register
&63 con EECR	\ EEPROM Control Register

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
&42  con ADC  \ ADC Conversion Complete
&44  con ERDY \ EEPROM Ready
&46  con ACI  \ Analog Comparator
&48  con TWI  \ Two-wire Serial Interface
&50  con SPM  \ Store Program Memory Read
