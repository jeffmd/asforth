\ LCD keypad shield driver
\ Module: lcd routines
\ uses the lcd module in 4bit mode
\  v 0.1
\ for asforth



\  The circuit:
\ * LCD RS pin to digital pin 8 PORTB 0
\ * LCD Enable pin to digital pin 9 PORTB 1
\ * LCD BL pin to digital pin 10 PORTB 2
\ * LCD D4 pin to digital pin 4 PORTD 4
\ * LCD D5 pin to digital pin 5 PORTD 5
\ * LCD D6 pin to digital pin 6 PORTD 6
\ * LCD D7 pin to digital pin 7 PORTD 7
\ * KEY pin to analog pin 0 PORTC 0



\ lcd-data - PORTD
\ lcd-ctrl - PORTB

\ commands
\ LCD_CLEARDISPLAY $01
\ LCD_RETURNHOME $02
\ LCD_ENTRYMODESET $04
\ LCD_DISPLAYCONTROL $08
\ LCD_CURSORSHIFT $10
\ LCD_FUNCTIONSET $20
\ LCD_SETCGRAMADDR $40
\ LCD_SETDDRAMADDR $80

\ flags for display entry mode
\ LCD_ENTRYRIGHT $00
\ LCD_ENTRYLEFT $02
\ LCD_ENTRYSHIFTINCREMENT $01
\ LCD_ENTRYSHIFTDECREMENT $00

\ flags for display on/off control
\ LCD_DISPLAYON $04
\ LCD_DISPLAYOFF $00
\ LCD_CURSORON $02
\ LCD_CURSOROFF $00
\ LCD_BLINKON $01
\ LCD_BLINKOFF $00

\ flags for display/cursor shift
\ LCD_DISPLAYMOVE $08
\ LCD_CURSORMOVE $00
\ LCD_MOVERIGHT $04
\ LCD_MOVELEFT $00

\ flags for function set
\ LCD_4BITMODE $02
\ LCD_2LINE $08
\ LCD_5x10DOTS $04
\ LCD_5x8DOTS $00

only I/O
vocabulary LCD
also LCD definitions

\ track lcd control status
cvar ctrl
\ track lcd mode status
cvar mode
\ current line for next print
cvar line

\ setup port pins for I/O
: sio
  \ setup pins 4,5,6,7 on Port D DDR for output
  $F0 DDRD rbs
  \ setup pins 0,1,2 on Port B DDR for output
  %00000111 DDRB rbs
  \ setup Timer1 for fast PWM using ICR1 as TOP
  \ use inverting output on OC1B (D10 port B 2) so brightness goes
  \ all the way off
  $1B32 TCCR1A !
  1000 ICR1H h!
  1000 OCR1BH h!
;

\ dim the light
: dim ( n -- )
  OCR1BH h!
;

\ pulse enable line of lcd
: pen
    \ lcd-en toggle low high low
    [ PORTB DIO 1 cbi, ]
    1 usec
    [ PORTB DIO 1 sbi, ]
    1 usec \ enable pulse must be >450ns
    [ PORTB DIO 1 cbi, ]
    40 usec \ commands need > 37us to settle
;

\ send high 4 bits of byte to lcd
: 4bs ( c -- )
    $F0 and
    $0F PORTD rbm
    pen
;

\ send a byte to lcd
: send ( c -- )
    dup 4bs
    swnib 4bs
;

\ send a command to lcd
: cmd ( c -- )
    \ lcd-rs low
    [ PORTB DIO 0 cbi, ]
    send
;

\ send data to lcd
: data ( c -- )
    \ lcd-rs high
    [ PORTB DIO 0 sbi, ]
    send
;

: reset
  \ lcd-rs low start off in command mode
  [ PORTB DIO 0 cbi, ]

  \ SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  \ according to datasheet, we need at least 40ms
  \ after power rises above 2.7V
  \ before sending commands. Arduino can turn on way
  \ before 4.5V so we'll wait 200 ms
  200 msec 
  
  \ put the LCD into 4 bit
    \ this is according to the hitachi HD44780 datasheet
    \ figure 24, pg 46

    \ we start in 8bit mode, try to set 4 bit mode
    $30 4bs 
    4100 usec \ wait min 4.1ms

    \ second try
    $30 4bs 
    100 usec
    
    \ third go!
    $30 4bs
    40 usec

    \ finally, set to 4-bit interface
    $20 4bs

  \ finally, set # lines, font size, etc.
  \ LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS
  %00101010  cmd

;

\ clear the lcd display
: clr
  1 cmd 2 msec
;

\ execute lcd display control command
: ctrlx
  ctrl c@ 8 or cmd
;

\ turn on flags in ctrl and send to lcd
: ctrl+ ( n -- )
  ctrl rbs ctrlx
;

\ turn off flags in ctrl and send to lcd
: ctrl- ( n -- )
  ctrl rbc ctrlx
;

\ turn display on
: on
   4 ctrl+
;

\ turn display off
: off
   4 ctrl-
;

\ turn blink on
: blink
   1 ctrl+
;

\ turn blink off
: blink-
   1 ctrl-
;

\ turn cursor on
: cur
   2 ctrl+
;

\ turn cursor off
: cur-
   2 ctrl-
;

\ move cursor to home position
: home
  2 cmd
  1 msec \ takes a while to complete
;


\ initialize lcd to a default working state
: init
  sio
  reset
  \ clear settings to default
  0 ctrl c!
  0 mode c!
  0 line c!
  off \ turn display off
  clr \ clear display
  Adc init
  LCD
;

\ move cursor to col, row position
: pos ( col row -- )
  ?if drop $40 then +
  $80 or cmd
;

\ turn backlight all the way on
: light
  [ PORTB DIO 2 sbi, ]
;

\ turn backlight off
: light-
  [ PORTB DIO 2 cbi, ]
;

\ sends character to lcd
\ used when defering system emit
: emit ( c -- )
  a swap data >a ;
;

: {.
  ['] emit Forth to emit
;

: .}
  ['] tx-poll to emit
  LCD
;
  
\ print ram string
: type ( addr len -- )
  {. type .}
;

\ print flash string
: itype ( addr len -- )
  {. itype .}
;

\ print a fixed point number
: .f ( fixedpoint -- )
  <# # # [char] . hold #s #> type

;

\ print a number
: . ( n -- )
  <# #s #> type
;
