\ analog to digital routines
\ adc registers
\ ADMUX $7C
\ ADCSRA $7A - adc control and status register A
\ ADCSRB $7B - adc control and status register B
\ ADCH $79 - adc data register high
\ ADCL $78 - adc data register low
\ DIDR0 $7E - digital input disable register 0


( -- val )
\ do adc conversion, puts 10 bit value on top of stack
: adc
\ start conversion, auto conversion is on
  \ start conversion
  %01000000 ADCSRA rbs
  \ wait ~200 usec
  200 usec
\ read adcl and adch by doing 16 bit read from adcl
  ADC @

;

( channel -- )
\ set adc mux to channel 
\ channel between 0 and 7 for external connections
\ channel 0 to 5 for atmega328 28pin dip
\ channel 0 to 7 for atmega328 32pin
\ channel 8 - on chip temperature sensor
\ channel 14 - 1.1v band gap
\ channel 15 - 0V ground
: amux
  $0F and
  $F0 ADMUX rbm
;

( ref -- )
\ set adc reference voltage
\ ref:
\      0 - AREF, internal Vref turned off
\      1 - AVcc with external cap at AREF pin
\      2 - Reserved
\      3 - internal 1.1V with external cap at AREF pin

: aref
  %11 and
  64 *
  $0F ADMUX rbm
;

( -- )
\ initialize the ADC to default values
: adcinit
\ disable digital inputs on first 5 analog inputs
%00111111 DIDR0 rbs
\ set voltage ref to AVcc
%01000000 ADMUX rbs
\ enable adc
\ set analog conversion to non free running mode
\ set prescaler to 128 to give 125K sample cycle
%10000111 ADCSRA rbs
;

( -- temperature )
\ get the temperature of the microcontroller in deg celcius
: temp
  \ get copy of AMUX
  ADMUX c@
  \ use internal 1.1V voltage ref
  \ 3 aref
  \ 8 amux \ set adc mux to channel 8
  %11001000 ADMUX c!
  \ give time for cap to change value when changing reference voltage
  10 msec
  adc
  \ formula to convert sensor val to celcius = (adc - Tos)/ k
  14 /
  \ restore AMUX
  swap ADMUX c!
;
