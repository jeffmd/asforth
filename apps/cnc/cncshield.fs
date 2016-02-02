\ cncshield.fs - vocabulary for stepper motor control that uses an
\ integrated driver ie Allegro A4988
\ 2 outputs for each stepper motor: step and dir

only I/O
vocabulary CNCS
also CNCS definitions

\ x axis stepper driver
: init ( -- )
  \ make PD2-7 outputs
  %11111100 DDRD rbs
  \ make PB0 output
  %1 DDRB rbs
;

\ enable stepper motors
: e ( -- )
  1 PORTB rbc
;

\ disable stepper motors
: d ( -- )
  1 PORTB rbs
;

\ stepper motor period delay between steps
cvar per

\ step the stepper motor using index
: step ( idx -- )
  \ do a 1 usec pulse
  4 PORTD rbs
  1 usec
  4 PORTD rbc
;

\ forward step
: f ( -- )
  idx c@ 1+ step
;

\ reverse step
: r ( -- )
  idx c@ 1- step
;


\ set direction forward
: df ( -- )
  %00100000 PORTD rbs
;

\ set direction reverse
: dr ( -- )
  %00100000 PORTD rbc
;

\ reverse direction
: rv ( -- )
  dir -1  * dir c!
;

\ step in forground in multiple times at per intervals in dir direction
: sm ( count -- )
  begin
    ?while 1- step per c@ msec repeat 
  drop
;

var uper

: fsm ( count -- )
  e
  begin
    ?while 1- step uper @ usec repeat
  drop
  d
;

\ full travel to rear and back to front
: ft
  r 16000 fsm f 16000 fsm
;

\ multiple full travels
: ftm ( count -- )
  begin
    ?while 1- ft repeat
  drop
;

\ accllerate for sm
: asm ( count -- )
  s 50 msec s 40 msec s 30 msec s 10 msec s 5 msec
  5 - sm
;

\ time in milliseconds since last drv execution
var lms
var sc

\ execute stepper driver
: xdrv
  \ if step count > 0 then step
  sc @ if -1 sc +! s then
;

\ old pause 
var op

\ background task driver place in pause
also Timer
: bdrv
  \ check if need to step based on rate
  ms @ lms @ - per c@ u> if ms @ lms ! xdrv then 
  op @ exec
;

\ setup driver background task and run
: run
  \ get the defer in pause
  ['] pause def@
  \ save the old pause
  op !
  ['] bdrv to pause
  ms @ lms !
;

\ background step
: bs ( count -- )
  sc !
;

\ move relative in inches
: mi ( inches100 -- )
  \ 18 tpi x 96 step per revolution
  1728 1000 */ bs
;

\ absolute position on x axis
var xpos
\ backlash
\ comes into play when there is a direction change
\ default to 11
cvar blsh

\ go to absolute position in thousands of an inch
\ 0 is left most position
: go ( pos -- )
  xpos @ over xpos ! -
  \ if negative  then dir is rev otherwise forward
  dup 0> if dir c@ df else abs dir c@ dr then
  \ account for backlash when there is a direction change
  dir c@ <> if blsh c@ + then
  mi
;
