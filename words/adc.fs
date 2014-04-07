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
\ start conversion
\ wait for conversion finished flag to be set
\ read adcl and adch by doing 16 bit read from adcl
  $78 @

;

( channel -- )
\ set adc mux to channel 
\ channel between 0 and 7 for external connections
\ channel 0 to 5 for atmega328 28pin dip
\ channel 8 - on chip temperature sensor
\ channel 14 - 1.1v band gap
\ channel 15 - 0V ground
: amux
  $0F and
  $7C \ admux is $7C
  >a
  ac@
  $F0 and or
  ac!
;

( -- )
\ initialize the ADC to default values
: adcinit
\ disable digital inputs on first 5 analog inputs
%00111111 $7E rbs
\ set voltage ref to AVcc
%01000000 $7C rbs
\ set analog conversion to free running mode
\ set prescaler to 128 to give 125K sample cycle
%11100111 $7A rbs
;

( -- temperature )
\ get the temperature of the microcontroller in deg celcius
: temp
  \ use internal 1.1V voltage ref
  8 amux \ set adc mux to channel 8
  adc
  \ formula to convert sensor val to celcius = (adc - Tos)/ k

;
