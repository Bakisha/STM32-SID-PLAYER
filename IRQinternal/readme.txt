


  // INFO:
  
  
  // Final version of STM32 SID Player:
  
  // Ability to play sid music from interrupt's, leaving main loop for a sketch. 
  
  //      - all checks are off, so first check if sid is playable from internalFlash version.
  //      - RAM for emulator is also limited (so sketch can at least have some)
  
  //      - only music based on raster irq (PSID V2+) (play address must not be 0, will not work with custom music player inside .sid file)
  //      - multi-speed .sid files are not supported, ( emulation of 6502 is not fast enough to keep up ).
  //      - .sid load address must be at $0400 or greater
  //      
  //
  // STM32F103C8
  // This project is purely for my own entertainment , so WITHOUT ANY WARRANTY!
  // I had tried to make it as much as posible to be portable for other microcontrollers
  // My choise of STM32F103C8 is only because of it's price
  // in bluepill's RAM, player is at 0x300, .sid file data is at 0x400 (if microcontrollers RAM is less then end of memory address of sid file, otherwise file data is in it's original memory address)
  // 


  // STM32F401CC board:
  //
  // next cheapest microcontroller. Much more RAM, but not full 65536 bytes. Around $D800 is last address that can be used (tunes that have higher end address will  be relocated)
  // tunes that work with it are in /f401 subfolder. Ofcorse, if tune in /f103 subfolder works on f103 microcontoller, it will work on f401 microcontoller too

  // STM32F411CC board:
  // Next in line of cheap microcontroller boards. Full 64K of RAM for emulator. (I personally overclocked mine to 110MHz, no USB serial, but Serial on PA9/PA10 works fine)





  // SCHEMATICS:
  //
  //
  //    .-----------------.
  //    |                 |
  //    | STM32FxxxXXxx   |
  //    .------------|----.
  //     |G         P|
  //     |N         A|
  //     |D         8--R1----|------C2---------|
  //     |                   |                 --
  //     |                   C                 || P1
  //     |                   1                 ||<--------- OUDIO OUT
  //     |                   |                 --
  //     .-------------------|------------------|---------- GND
  //                        GND
  //    R1 = 100-500 Ohm
  //    C1 = 100 nF
  //    C2 = 10 uF
  //    P1 = 10KOhm potentiometer
  //
  // If <period> is 1 , AUDIO OUT can be connected to PA8 (no need for R1,C1 ). I don't think 1Mhz sample rate will be in hearing range
