\ io.fs - input output port words

$25 con PORTB
$24 con DDRB
$23 con PINB

$28 con PORTC
$27 con DDRC
$26 con PINC

$2B con PORTD
$2A con DDRD
$29 con PIND

\ turn port into direct i/o address
: DIO ( port -- )
  $20 -
;
