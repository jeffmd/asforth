\ LCD keypad shield driver
\ Module: lcd routines
\ use the lcd module in 4bit mode
\  v 0.1

\ needs marker.frt and bitnames.frt from lib

marker _lcd_

\ helper routine
: ms 0 ?do 1ms loop ;

hex
(
  The circuit:
 * LCD RS pin to digital pin 8 PORTB 0
 * LCD Enable pin to digital pin 9 PORTB 1
 * LCD D4 pin to digital pin 4 PORTD 4
 * LCD D5 pin to digital pin 5 PORTD 5
 * LCD D6 pin to digital pin 6 PORTD 6
 * LCD D7 pin to digital pin 7 PORTD 7
 * LCD BL pin to digital pin 10 PORTB 2
 * KEY pin to analogl pin 0 PORTC 0
)

1b 20 + con lcd-data \ PORTD
18 20 + con lcd-ctrl \ PORTB

lcd-ctrl 1 portpin: lcd-rw
lcd-ctrl 0 portpin: lcd-en
lcd-ctrl 2 portpin: lcd-rs

2 con lcd-pulse-delay
a con lcd-short-delay

: lcd-pulse-en
    lcd-en high
    lcd-pulse-delay ms
    lcd-en low
    lcd-pulse-delay ms
;

: lcd-data-mode
    lcd-rs high
;

: lcd-command-mode
    lcd-rs low
;


: lcd-read-mode
    0 lcd-data 1- c! \ input
    lcd-rw high
;

: lcd-write-mode
    ff lcd-data 1- c! \ output
    lcd-rw low
;

: lcd-read-data ( -- c )
	lcd-read-mode
	lcd-pulse-en
	lcd-short-delay ms
	lcd-data 1- 1- c@ 
;

: lcd-wait
    lcd-read-mode
    lcd-rw high
    lcd-rs low
    lcd-pulse-en
    begin
        lcd-data 1- 1- c@
	80 and
    until
;

: lcd-command ( n -- )
    lcd-wait
    lcd-write-mode
    lcd-command-mode
    lcd-data c!
    lcd-pulse-en
;

: lcd-emit ( c -- )
    lcd-write-mode
    lcd-data-mode
    lcd-data c!
    lcd-pulse-en
;

: lcd-init
    lcd-rw pin_output
    lcd-en pin_output
    lcd-rs pin_output
;
\ from tracker: lcd.frt - added LCD initialization - ID: 2785157
: lcd-cmd-no-wait ( n -- )
  lcd-write-mode
  lcd-command-mode
  lcd-data c!
  lcd-pulse-en
;

: lcd-start
  lcd-init
  15 ms
  30 lcd-cmd-no-wait
  4 ms
  30 lcd-cmd-no-wait
  1 ms
  30 lcd-cmd-no-wait
  38 lcd-command
  6 lcd-command
  c lcd-command
  1 lcd-command
;


: lcd-page ( clear page )
    1 lcd-command ( clear lcd )
    3 lcd-command ( cursor home )
;

