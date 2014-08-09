\ LCD keypad shield driver
\ Module: lcd routines
\ uses the lcd module in 4bit mode
\  v 0.1
\ for asforth


(
  The circuit:
 * LCD RS pin to digital pin 8 PORTB 0
 * LCD Enable pin to digital pin 9 PORTB 1
 * LCD BL pin to digital pin 10 PORTB 2
 * LCD D4 pin to digital pin 4 PORTD 4
 * LCD D5 pin to digital pin 5 PORTD 5
 * LCD D6 pin to digital pin 6 PORTD 6
 * LCD D7 pin to digital pin 7 PORTD 7
 * KEY pin to analog pin 0 PORTC 0
)

\ PORTB $05 ($25)
\ DDRB  $04 ($24)
\ PINB  $03 ($23)

\ PORTC $08 ($28)
\ DDRC  $07 ($27)
\ PINC  $06 ($26)

\ PORTD $0B ($2B)
\ DDRD  $0A ($2A)
\ PIND  $09 ($29)

\ $0B ($2B) lcd-data - PORTD
\ $05 ($25) lcd-ctrl - PORTB

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

\ track lcd control status
cvar lcd.ctrl
\ track lcd mode status
cvar lcd.mode
\ current line for next print
cvar lcd.line

\ setup port pins for I/O
: lcd.sio
  \ setup pins 4,5,6,7 on Port D DDR for output
  %11110000 $2A rbs
  \ setup pins 0,1,2 on Port B DDR for output
  %00000111 $24 rbs
;

\ pulse enable line of lcd
: lcd.pen
    \ lcd-en toggle low high low
    [ $05 1 cbi, ]
    1 usec
    [ $05 1 sbi, ]
    1 usec \ enable pulse must be >450ns
    [ $05 1 cbi, ]
    40 usec \ commands need > 37us to settle
;

\ send high 4 bits of byte to lcd
: lcd.4bs ( c -- )
    %11110000 and
    $2B >a ac@ %00001111 and or
    ac! lcd.pen
;

\ send a byte to lcd
: lcd.send ( c -- )
    dup lcd.4bs
    swnib lcd.4bs
;

\ send a command to lcd
: lcd.cmd ( c -- )
    \ lcd-rs low
    [ $05 0 cbi, ]
    lcd.send
;

\ send data to lcd
: lcd.data ( c -- )
    \ lcd-rs high
    [ $05 0 sbi, ]
    lcd.send
;

: lcd.reset
  \ lcd-rs low start off in command mode
  [ $05 0 cbi, ]

  \ SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  \ according to datasheet, we need at least 40ms after power rises above 2.7V
  \ before sending commands. Arduino can turn on way befer 4.5V so we'll wait 200 ms
  200 msec 
  
  \ put the LCD into 4 bit
    \ this is according to the hitachi HD44780 datasheet
    \ figure 24, pg 46

    \ we start in 8bit mode, try to set 4 bit mode
    $30 lcd.4bs 
    4100 usec \ wait min 4.1ms

    \ second try
    $30 lcd.4bs 
    100 usec
    
    \ third go!
    $30 lcd.4bs
    40 usec

    \ finally, set to 4-bit interface
    $20 lcd.4bs

  \ finally, set # lines, font size, etc.
  \ LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS
  %00101010  lcd.cmd

;

\ clear the lcd display
: lcd.clr
  1 lcd.cmd 2 msec
;

\ execute lcd display control command
: lcd.ctrlx
  lcd.ctrl c@ 8 or lcd.cmd
;

\ turn on flags in ctrl and send to lcd
: lcd.ctrl+ ( n -- )
  lcd.ctrl rbs lcd.ctrlx
;

\ turn off flags in ctrl and send to lcd
: lcd.ctrl- ( n -- )
  lcd.ctrl rbc lcd.ctrlx
;

\ turn display on
: lcd.on
   4 lcd.ctrl+
;

\ turn display off
: lcd.off
   4 lcd.ctrl-
;

\ turn blink on
: lcd.blink
   1 lcd.ctrl+
;

\ turn blink off
: lcd.blink-
   1 lcd.ctrl-
;

\ turn cursor on
: lcd.cur
   2 lcd.ctrl+
;

\ turn cursor off
: lcd.cur-
   2 lcd.ctrl-
;

\ move cursor to home position
: lcd.home
  2 lcd.cmd
;

\ turn cursor off

\ initialize lcd to a default working state
: lcd.init
  lcd.sio
  lcd.reset
  \ clear settings to default
  0 lcd.ctrl c!
  0 lcd.mode c!
  0 lcd.line c!
  lcd.off \ turn display off
  lcd.clr \ clear display
;

\ move cursor to col, row position
: lcd.pos ( col row -- )
  ?if drop $40 then +
  $80 or lcd.cmd
;
