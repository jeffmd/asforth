\ clock.fs
\ uses tasker to keep track of days, hours, minutes, seconds since starting

only Tasker
vocabulary Clock
also Clock definitions

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
  23 hrs 1+c!mx if days+ then
;

\ increment number of minutes by one
: mins+ ( -- )
  59 mins 1+c!mx if hrs+ then
;

\ increment number of seconds by one
: secs+ ( -- )
  59 secs 1+c!mx if mins+ then
;

\ clear the clock
: clr
  0 dup days !
  dup hrs c!
  dup mins c!
  secs c!
;

\ run the clock
: run ( -- )
  ['] secs+ 14 task!
;

: stop ( -- )
  14 taskclr
;
