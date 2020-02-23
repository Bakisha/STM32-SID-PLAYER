 SID emulation only
 I use it for my projects that need simple sounds or sound effects.

 This project is purely for my own entertainment , so WITHOUT ANY WARRANTY!
 
 SCHEMATICS:


    .-----------------.
    |                 |
    | STM32FxxxXXxx   |
    .------------|----.
     |G         P|
     |N         A|
     |D         8--R1----|------C2---------|
     |                   |                 --
     |                   C                 || P1
     |                   1                 ||<--------- OUDIO OUT
     |                   |                 --
     .-------------------|------------------|---------- GND
                        GND
    R1 = 100-500 Ohm
    C1 = 100 nF
    C2 = 10 uF
    P1 = 10KOhm potentiometer

 If <period> is 1 , AUDIO OUT can be connected to PA8 (no need for R1,C1 ). I don't think 1Mhz sample rate will be in hearing range

Detailed information of how to control SID chip:

http://archive.6502.org/datasheets/mos_6581_sid.pdf