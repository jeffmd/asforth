asForth: a subroutine threaded forth writtin in AVR GNU assembly to be compiled with gnu avr assembler.

Based on amForth, FlashForth, AVRForth, CamelForth, TurboForth, eForth, FigForth

Converted amForth from indirect threaded code to subroutine threaded.  This resulted in a speed up of 4 to 8 times faster than indirect threaded code.  used some features from FlashForth for inlining words that are less than 4 words in code length. 

Authours:
 amForth: Andy Kirby (andy@kirbyand.co.uk), Based on the Amforth Application Template.
Jeff Doyle: conversion of amForth to subroutine threaded.
Licensing: as per the Amforth Project.


asforth can be flashed onto the following MCU:

Model		Microcontroler	Host	Xtal	DBG-LED	Flash	B-Load (words)    	Ram	Fuses (E,H,L)
Duemilanove	ATMega 328	    uart0	16Mhz	PB5	    32k	    256b/512b/1k/2k		2k	05 D9 FF
Uno         ATMega 328	    uart0	16Mhz	PB5	    32k	    256b/512b/1k/2k		2k	05 D9 FF


Notes

1. Double check the fuses settings. Esp. the duemilanove may have the wrong settings. set the HFuse to 0xd9
   to maximize the bootloader size.

2. Whilst most errors and problems you encounter are likely to be those I have created rather than the original 
   code on which this is based, please report forward comments, feedback, reports, bugs, fixes and patches etc 
   through the Amforth Projects development mailing lists and forums etc.

3. The binary asforth images cannot be loaded/programmed using the Arduino Bootloader. An ICSP programmer 
   (avrisp, etc) must be used to load the image.

4. The Arduino bootloader is over writen with the asforth code and is no longer available after programing. 
   To restore your board for use with the Arduino IDE you must overwrite the asforth image with an Arduino 
   Bootloader image.
 
5. Whilst described as using a 328 device early versions of the Duemilanove may actualy have a 168 installed. 
   This can be easily exchanged for a 328 if more resources are needed.  

6. The Diecimila board is also compatible with the 328 device commonly found in the newer Duemilanove board.

   
7. The UNO has the same controller as the duemilanove, the hexfiles are the same.
