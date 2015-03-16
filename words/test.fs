only LCD also Clock also Tasker
vocabulary Trial
also Trial definitions

\ debug flasher task
: flash count 1 and if dbg+ else dbg- then ;

\ display 2 digit number
: .c c@ <# # # #> type ;
\ display clock seconds at top right
: dsec 14 0 pos secs .c ;
\ display lcd key val at bottom right
: dkey 12 1 pos 0 Adc amux conv . bl emit ; Trial

: start
  reset
  ['] flash 13 task!
  ['] dsec 12 task! 
  ['] dkey 11 task!
  Clock clr run
  LCD init on 999 dim
  Trial
;

' start is turnkey
