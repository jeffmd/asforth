\ tasker.fs : words for managing tasks

\ the active index into the task list
cvar tidx

\ count register for each task: max 32 tasks
\ is an array of 32 bytes
cvar tcnt
31 allot

( -- n )
\ fetch task index: verifies index is valid
\ adjusts index if count is odd ?
: tidx@
  tidx c@ 
  \ verify index is below 32
  dup 31 >
  if
    \ greater than 31 so 0
    0:
    dup tidx c!
  then
;

( idx -- cnt )
\ get count for a slot
\ idx: index of slot
: tcnt@
  tcnt + c@
;

\ increment tcnt array element using idx as index
( idx -- )
: tcnt+
  tcnt + 1+c!
;

\ array of task slots in eeprom : max 32 tasks 64 bytes
edp con tasks
edp 64 + to edp

( -- )
\ increment task index to next task idx
\ assume array flat layout and next idx = idx*2 + 1
: tidx+
  tidx@ 2* 1+ 
  \ if slot count is odd then 1+
  tidx@ tcnt@
  1 and +
  tidx c!
;

( idx -- task )
\ get a task at idx slot
: task@
  2* tasks + @e 
;

( idx -- ) ( C:task_name )
\ store a task in a slot
\ idx is the slot index range: 0 to 31
: task
  2*
  tasks +
  '
  swap !e
;

( -- )
\ execute active task and step to next task
: taskex
  \ increment count for task slot
  tidx@ tcnt+
  tidx@ task@ exec
  tidx+
;

\ time in ms since last taskex
var taskms

( -- )
\ execute taskex every 125 ms
\ if count == 125 then taskex reset count
\ else inc count
\ ms gets updated every 1 ms
\ how to set up trigger
\ taskms = old ms
: tasker
 ms @ taskms @ - 125 u> if ms @ taskms ! taskex then 
;
