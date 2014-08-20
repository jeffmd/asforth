\ servo words

\ set pwm for servo on timer1 using an angle
( angle -- )
: srvo
  \ angle conversion to timer ticks
  \ servo pulse times and ticks
  \  us     ticks     degrees
  \ 600      150         0
  \1500      375        90
  \2400      600       180
  
  25 10 */ 150 + OCR1AH h!
;

\ initialize timer1 for servo control
( -- )
: initsrvo
  \ enable OC1A as output for PWM
  \ Port B bit 1
  2 DDRB rbs
  \ set IRC1 for 50 hz
  4999 ICR1H h!
  \ defualt pwm to 0
  0 srvo
  \ set timer1 for fast PWM using ICR1 for TOP
  \ and ioclk/64
  $1B82 TCCR1A !
;
