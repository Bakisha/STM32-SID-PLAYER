 SID chip and 6502 CPU emulator, with audio output on pin PA8 (PWM) or PA4 (DAC), for STM32 line of microcontrollers, compiled with Arduino IDE
  My personal breadboard project that i'm still having fun with . 
      - Mainly done for STM32F103C8 Blue Pill (PWM) and STM32G431CBU6 (WeAct board) (DAC)
      - Should work with other STM32 microcontrollers that Arduino IDE support.
      - Tested also at STM32F401CCU6, STM32F411CEU6 and STM32F407VET6 (PWM/DAC) dev boards (and as i see, they all have same pin (PA8) for Timer1,channel1) and same pins for SPI.

  -Only PSID V2 sids, no digis, emulator is not fast enough.

  -Maximum size of sid is limited by microcontrollers RAM

  -Sid load address must be at $0400 or greater.

  -Filter emulation is far from real SID chip, but, for me, it sound enough "SID-ish".

  -Plays sid from hardcoded folders from SD CARD. Use buttons or infrared remote control (NEC protocol) to switch to next tune/file/folder.

  -SID tune database can be found at  https://www.hvsc.c64.org/

  I'm not good at C, so code is maybe a mess, but it works :-) Code used for emulator is found at http://rubbermallet.org/fake6502.c

  reSID can be found at https://en.wikipedia.org/wiki/ReSID. This is not reSID port, but i did used some parts of reSID code.


  SCHEMATICS (not to scale) :

  PWM: 
  
  STM32F103C8/B - STM32F401CC - STM32F411CE :

  .------------------------------------------------------.                                              +----------+
  |                                                      |--------< 3.3V  >-----------------------------| infrared |
  | STM32FxxxXXxx                                        |--------< irPIN >-----------------------------| receiver |
  .-----------------------|------|------|----|-----------.--------< GND   >-----------------------------|          |
  |G P   P P P            P      P      P   P|                                                          +----------+
  |N A   A A A            B      B      B   A|
  |D 1   5 6 7            0      1     10   8.------|R1|---+---|C2|----------|
  |  |   | | -- SD_MOSI   |      |      |                  |                 --
  |  |   | ---- SD_MISO   |.     |.     |.                 C                 || P1
  |  |   ------ SD_CLK     /SW    / SW   /SW               1                 ||<---| OUDIO OUT
  |  ---------- CS_SDARD  |      |      |                  |                 --
  .-----------------------+------+------+------------------+-----------------|------------------| GND
                                       GND


  STM32F407VET6 black board   :

  .------------------------------------------------------.                                              +----------+
  |                                                      |--------< 3.3V  >-----------------------------| infrared |
  | STM32F407VE                                          |--------< irPIN >-----------------------------| receiver |
  .-----------------------|------|------|----|-----------.--------< GND   >-----------------------------|          |
  |G P   P P P            P      P      P   P|                                                          +----------+
  |N B   B B B            B      B      B   A|
  |D 7   3 4 5            0      1     10   8.------|R1|---+---|C2|----------|
  |  |   | | -- SD_MOSI   |      |      |                  |                 --
  |  |   | ---- SD_MISO   |.     |.     |.                 C                 || P1
  |  |   ------ SD_CLK     /SW    / SW   /SW               1                 ||<----------------| OUDIO OUT
  |  ---------- CS_SDARD  |      |      |                  |                 --
  .-----------------------+------+------+------------------+-----------------|------------------| GND
  R1 = 100-500 Ohm
  C1 = 10-100 nF
  C2 = 10 uF
  P1 = 10KOhm potentiometer

  DAC :

  STM32G41CB :

  .------------------------------------------------------.                                              +----------+
  |                                                      |--------< 3.3V  >-----------------------------| infrared |
  | STM32FxxxXXxx                                        |--------< irPIN >-----------------------------| receiver |
  .-----------------------|------|------|----|-----------.--------< GND   >-----------------------------|          |
  |G P   P P P            P      P      P   P|                                                          +----------+
  |N C   A A A            B      B      B   A|
  |D 4   5 6 7            0      1     10   4.----------------------|
  |  |   | | -- SD_MOSI   |      |      |                           --
  |  |   | ---- SD_MISO   |.     |.     |.                          || P1
  |  |   ------ SD_CLK     /SW    / SW   /SW                        ||<------|C|-------| OUDIO OUT
  |  ---------- CS_SDARD  |      |      |                           --
  .-----------------------+------+------+---------------------------|------------------| GND
                                       GND


  STM32F407VET6 black board   :

  .------------------------------------------------------.                                              +----------+
  |                                                      |--------< 3.3V  >-----------------------------| infrared |
  | STM32F407VE                                          |--------< irPIN >-----------------------------| receiver |
  .-----------------------|------|------|----|-----------.--------< GND   >-----------------------------|          |
  |G P   P P P            P      P      P   P|                                                          +----------+
  |N B   B B B            B      B      B   A|
  |D 7   3 4 5            0      1     10   4.-------------------------------|
  |  |   | | -- SD_MOSI   |      |      |                                    --
  |  |   | ---- SD_MISO   |.     |.     |.                                   || P1
  |  |   ------ SD_CLK     /SW    / SW   /SW                                 ||<------|C|-------| OUDIO OUT
  |  ---------- CS_SDARD  |      |      |                                    --
  .-----------------------+------+------+------------------------------------|------------------| GND
  C = 10 uF
  P1 = 10KOhm potentiometer



  HAVE FUN :-)
