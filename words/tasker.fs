\ tasker.fs : words for managing tasks

only Timer
vocabulary Tasker
also Tasker definitions

\ the active index into the task list
cvar tidx

\ count register for each task: max 31 tasks
\ is an array of 31 bytes
cvar tcnt
30 allot

( -- n )
\ fetch task index: verifies index is valid
\ adjusts index if count is odd ?
: tidx@
  tidx c@ 
  \ verify index is below 31
  dup 30 >
  if
    \ greater than 30 so 0
    0:
    dup tidx c!
  then
;

( idx -- cnt )
\ get count for a slot
\ idx: index of slot
: cnt@
  tcnt + c@
;

\ increment tcnt array element using idx as index
( idx -- )
: cnt+
  tcnt + 1+c!
;

( n idx -- )
\ set tcnt array element using idx as index
: cnt!
  tcnt + c!
;

\ array of task slots in eeprom : max 31 tasks 62 bytes
\ array is a binary process tree
\                        0                          125 ms
\             1                      2              250 ms
\      3           4           5           6        500 ms
\   7     8     9    10     11   12     13   14     1 s
\ 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30   2 s
edp con tasks
edp 62 + to edp

( -- )
\ increment task index to next task idx
\ assume array flat layout and next idx = idx*2 + 1
: tidx+
  tidx@ 2* 1+ 
  \ if slot count is odd then 1+
  tidx@ cnt@
  1 and +
  tidx c!
;

( idx -- task )
\ get a task at idx slot
: task@
  2* tasks + @e 
;

( addr idx -- ) 
\ store a task in a slot
\ idx is the slot index range: 0 to 30
: task!
  2*
  tasks +
  !e
;

( idx -- )
\ clear task at idx slot
\ replaces task with noop
: taskclr 
  ['] noop swap task!
;

( -- )
\ execute active task and step to next task
: taskex
  \ increment count for task slot
  tidx@ cnt+
  tidx@ task@ exec
  tidx+
;

\ time in ms since last tasks.ex
var lastms
\ how often in milliseconds to execute a task
\ default to 25 ms : 7.8 counts per ms
195 val exms


( -- )
\ execute tasks.ex if tick time expired
: tick
  ms @ lastms @ - exms u> if ms @ lastms ! taskex then 
;

( -- )
\ clear all tasks
: allclr
  \ iterate 0 to 30 and clear tcnt[] and set tasks[] to noop
  0
  begin
    0 tidx c!
    0 over cnt!
    dup taskclr 
    1+ 
    dup 30 >  
  until
  drop
;

( -- )
\ start tasking
: run
  \ set taskms to ms
  T0init
  ms @ lastms !
  ['] tick to pause
;

( -- )
\ reset tasker
\ all tasks are reset to noop
: reset
  allclr
  run
;

( -- )
\ stop tasks from running
: stop
  ['] noop to pause
;
