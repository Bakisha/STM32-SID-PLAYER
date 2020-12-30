SID chip (6581 and 8580) and 6502 CPU emulator, with audio output on pin PA8, for STM32 line of microcontrollers, compiled with Arduino IDE, uploaded with ST-LINK V2.

Demonstration video: https://youtu.be/pvWagY7ppYg

My personal project that i was having fun for some time now. Mainly done for STM32F103C8 (Blue Pill). Should work with other STM32 microcontrollers that Arduino IDE support. Tested also at STM32F401CCU6 dev board (and as i see, they all have same pin (PA8) for Timer1,channel1)

-Only IRQ based sids can be played (no digis, emulator is not fast enough).

-Maximum size of sid is limited by microcontrollers RAM (14336 bytes for STM32F103C8T6, 62464 bytes for STM32F401CCU6).

-Sid load address must be at $0400 or greater.

-Filter emulation is far from real SID chip, but, for me, it sound enough "SID-ish".

-Plays sid from internal flash memory. Sid file is converted to array. Converter used is bin2c that can be found at https://sourceforge.net/projects/bin2c/files/

-Converted files can be downloaded from https://www.mediafire.com/file/w0txoba0pbl5led/HVSC74.zip/file

-It is configurable. On first tab in Arduino IDE is values that can be changed to match microcontroller used. Currently it's set for STM32F103C8.

I'm not good at C, so code is maybe a mess, but it works :-) Code used for emulator is found at http://rubbermallet.org/fake6502.c

reSID can be found at https://en.wikipedia.org/wiki/ReSID. This is not reSID port, but i did used some parts of reSID code.

Much more details are in the sketch.

SCHEMATICS (not to scale) :

STM32F103C8/B - STM32F401CC - STM32F411CE :

  .-------------------------------------.
  |                                     |
  | STM32FxxxXXxx                       |
  .---------------------------|-----|---.
  |G                          P    P|
  |N                          B    A|
  |D                          0    8-----|R1|------|C2|----------|
  |                           |                |                 --
  |                           |.               C                 || P1
  |                            / SW            1                 ||<----------------| OUDIO OUT
  |                           |                |                 --
  .---------------------------|----------------|-----------------|------------------| GND
                                             GND


  STM32F407VET6 black board   :

  .-------------------------------------.
  |                                     |
  | STM32F407VE                         |
  .---------------------------|-----|---.
  |G                          P    P|
  |N                          E    A|
  |D                          3    8-----|R1|------|C2|----------|
  |                           |                |                 --
  |                           |.               C                 || P1
  |                            / SW            1                 ||<----------------| OUDIO OUT
  |                           |                |                 --
  .---------------------------|----------------|-----------------|------------------| GND
                                             GND
  R1 = 100-500 Ohm
  C1 = 10-100 nF
  C2 = 10 uF
  P1 = 10KOhm potentiometer




HAVE FUN :-)
