only I/O also Clock also Tasker
vocabulary Turbine
also Turbine definitions

\ turbine RPM pulse count
var trpmpc

\ battery voltage
var batv

\ voltage into controller
var vin
\ current into battery
var iin
\ current out of battery into load
var iout

\ last key pressed
cvar lkey

\ setup I/O pins 
: sio ( -- )
  \ Port D 2 : arduino digital pin 2 - input 
  %0100 dup DDRD rbc
  \ use pullup resistor on port D 2
  PORTD rbs
  \ Port B 4 : arduino digital pin 12 - output
  \ Port B 3 : arduino digital pin 11 - output
  %11000 DDRB rbs
;

\ rpm pin change interrupt
:isr trpmpci ( -- )
  trpmpc 1+!
;isr

\ setup interrupt vector for rpmpci
\ use vector: for pin 0
INT0 isr trpmpci
  
\ turbine rpm init
: init ( -- )
  sio
  trpmpc 0! batv 0!
  \ detect rising edge on INT0
  %0011 EICRA rbs
  \ enable interrupt for pin 0 change on rising edge
  %0001 EIMSK rbs
;

\ analog conv high voltage 16v
: achv ( chan -- conv )
  Adc amux conv 157 100 */ Turbine
;
\ analog conv max 5.11 voltage
: ac5v ( chan -- conv )
  Adc amux conv 2/
;

Turbine

\ refresh batv from adc
: batv~ ( -- )
  1 achv batv !
;

\ refresh batv from adc
: vin~ ( -- )
  2 achv vin !
;

\ refresh iin
: iin~ ( -- )
  3 ac5v iin !
;

\ refresh iout
: iout~ ( -- )
  4 ac5v iout !
;

\ convert key analog value to key digit
\ get analog value
: scankey ( -- )
  0 Adc amux conv Turbine lkey c!
\ figure out which key was pressed base on analog value
\ no key > 1000
\ 
;

\ Turn load on
: ld+ ( -- )
  [ PORTB DIO 4 sbi, ]
;

\ Turn load off
: ld- ( -- )
  [ PORTB DIO 4 cbi, ]
;

\ Turn charge on
: ch+ ( -- )
  [ PORTB DIO 3 sbi, ]
;

\ Turn charge off
: ch- ( -- )
  [ PORTB DIO 3 cbi, ]
;
 
also LCD

\ display trpmpc on LCD
: drpmpc ( -- )
  0 0 pos trpmpc @ .
;

\ display batv in fixed notation on LCD
: dbatvf ( -- )
  0 1 pos batv~ batv @ .f
;

\ display batv in fixed notation on LCD
: dvinf ( -- )
  6 1 pos vin~ vin @ .f
;

\ display iin in fixed notation on LCD
: diinf ( -- )
  0 0 pos iin~ iin @ .f
;

\ display iout in fixed notation on LCD
: dioutf ( -- )
  6 0 pos iout~ iout @ .f
;

\ start up the display tasks for turbine
: start
  ['] dbatvf 7 task!
  ['] dvinf 8 task!
  ['] diinf 9 task!
  ['] dioutf 10 task!
;
