\ clock.fs
\ uses tasker to keep track of days, hours, minutes, seconds since starting

var days
cvar hrs
cvar mins
cvar secs

\ increment number of days by one
: days+ ( -- )
  days 1+!
;

\ increment number of hours by one
: hrs+ ( -- )
  hrs c@ 1+ dup 23 > if 0: days+ then hrs c!
;

\ increment number of minutes by one
: mins+ ( -- )
  mins c@ 1+ dup 59 > if 0: hrs+ then mins c!
;

\ increment number of seconds by one
: secs+ ( -- )
  secs c@ 1+ dup 59 > if 0: mins+ then secs c!
;


\ clear the clock
: clock.clr
  0 dup days !
  dup hrs c!
  dup mins c!
  secs c!
;

\ run the clock
: clock.run ( -- )
  ['] secs+ 14 task!
;

: clock.stop ( -- )
  14 task.clr
;
