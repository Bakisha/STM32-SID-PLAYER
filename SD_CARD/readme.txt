SID chip (6581 and 8580) and 6502 CPU emulator, with audio output on pin PA8, for STM32 line of microcontrollers, compiled with Arduino IDE, uploaded with ST-LINK V2.



My personal project that i was having fun for some time now. Mainly done for STM32F103C8 (Blue Pill). Should work with other STM32 microcontrollers that Arduino IDE support. Tested also at STM32F401CCU6 dev board (and as i see, they all have same pin (PA8) for Timer1,channel1) and same pins for SPI.

-Only single speed IRQ based sids can be played (PSID V2 sids, no digis, emulator is not fast enough).

-Maximum size of sid is limited by microcontrollers RAM (13312 bytes for STM32F103C8T6, 57344 bytes for STM32F401CCU6).

-Sid load address must be at $0400 or greater.

-Filter emulation is far from real SID chip, but, for me, it sound enough "SID-ish".

-Plays sid from folder from SD CARD. Use button to switch to next tune/file.

-SID tune database can be found at  https://www.hvsc.c64.org/

-It is configurable. On first tab in Arduino IDE is values that can be changed to match microcontroller used. Currently it's set for STM32F103C8.

I'm not good at C, so code is maybe a mess, but it works :-) Code used for emulator is found at http://rubbermallet.org/fake6502.c

reSID can be found at https://en.wikipedia.org/wiki/ReSID. This is not reSID port, but i did used some parts of reSID code.


SCHEMATICS (not to scale) :

STM32F103C8/B - STM32F401CC - STM32F411CE :

  .-------------------------------------.
  |                                     |
  | STM32FxxxXXxx                       |
  .----------------------------|-----|--.
  |G P   P P P                P    P|
  |N A   A A A                B    A|
  |D 1   5 6 7                0    8-----|R1|------|C2|----------|
  |  |   | | -- SD_MOSI       |                |                 --
  |  |   | ---- SD_MISO       |.               C                 || P1
  |  |   ------ SD_CLK         / SW            1                 ||<----------------| OUDIO OUT
  |  ---------- CS_SDARD      |                |                 --
  .---------------------------|----------------|-----------------|------------------| GND
                                             GND


  STM32F407VET6 black board   :

  .-------------------------------------.
  |                                     |
  | STM32F407VE                         |
  .----------------------------|-----|--.
  |G P   P P P                P    P|
  |N B   B B B                E    A|
  |D 7   3 4 5                3    8-----|R1|------|C2|----------|
  |  |   | | -- SD_MOSI       |                |                 --
  |  |   | ---- SD_MISO       |.               C                 || P1
  |  |   ------ SD_CLK         / SW            1                 ||<----------------| OUDIO OUT
  |  ---------- CS_SDARD      |                |                 --
  .---------------------------|----------------|-----------------|------------------| GND
                                             GND
  R1 = 100-500 Ohm
  C1 = 10-100 nF
  C2 = 10 uF
  P1 = 10KOhm potentiometer




HAVE FUN :-)
