// Choose your board and upload method from menu
// Choose available RAM for emulator (depending of microcontroller) (currently set for BluePill - STM32F103C8, any core )
// it's strongly recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE). 


////////////////////////////////////////////////////////////////////////////////////////////
//
//           emulator settings 
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define RAM_SIZE 0x3000                   // set this value based on microcontroller used. maximum is 65535 bytes ( 0xFFFF HEX ) or available microcontoller's RAM
#define TUNE_PLAY_TIME 360                // Can't implement songlenghts, manual values are needed (in seconds)//  TODO: try to determine silence in output, and skip to next tune
uint8_t DEFAULT_SONG = 0;                 // 0 is automatic, from sid header, any other value is tune number

/////////////////////////////////////////////////////////////////////////////////////////////
//
// copy-paste tune's path here (from tunes.h or HVSC.h)
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include"tunes/f103/CJs_Elephant_Antics.h"

////////////////////////////////////////////////////////////////////////////////////////////
//
//          optional hardware settings 
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define USE_SERIAL                  //       for debugging info on Serial , uncomment if it's needed
#define USE_SERIAL1                 //       for debugging info on Serial1, uncomment if it's needed


////////////////////////////////////////////////////////////////////////////////////////////
//
//          optional emulator settings 
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define USE_FILTERS                        // uncomment for testing, irq is  faster in calculations 

#define FILTER_SENSITIVITY  4             // Lower is better. Best is 1.  It will consume irq time , but FILTER_FREQUENCY can be set much higher
#define FILTER_FREQUENCY 12500             // You'll know it if it's very high (it will totaly ruin sound)

//                                            Play with these two values to determine what is "SID-like" sound
//                                            NOTE: Filters are linear in all frequencies range. Don't ask for true filters emulation, i'm suprised i managed to emulate it at all :-)
//                                         --------------------------------------------
//                                         |                      |  maximum            |
//                                         |  FILTER_SENSITIVITY  |  FILTER_FREQUENCY   |
//                                         ----------------------------------------------
//                                         |            1         |      18000          |
//                                         ----------------------------------------------
//                                         |            2         |      14000          |
//                                         ----------------------------------------------
//                                         |            4         |      12500          |
//                                         ----------------------------------------------
//                                         |            8         |       8000          |
//                                         ----------------------------------------------
//                                         |            16        |       4000          |
//                                         ----------------------------------------------



#define USE_CHANNEL_1                     // uncomment, for testing (affect only volume, not the calculations)
#define USE_CHANNEL_2                     // (but it will have effect on filter output in case of multiple voices is filtered)
#define USE_CHANNEL_3                     // 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Don't change stuff bellow
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_HAL_DRIVER // Official ST cores. Support for multiple line of MPU
#define USE_STM32duino_CORE //  Set in preferences: https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json and search stm32 in board manager. Choose stm32 cores by ST Microelectronics. Select your CPU from tools menu)
#else
#define USE_ROGER_CORE //  Set in preferences: http://dan.drown.org/stm32duino/package_STM32duino_index.json and search stm32F1 in board manager. Choose STM32F1xx core (NOTE: Only STM32F1 works)
#endif

#include "SID.h"
#include "6502.h"

// INFO:

//      - only music based on raster irq (PSID V2+) (play address must not be 0, will not work with custom music player inside .sid file)
//      - multi-speed .sid files are not supported, ( emulation of 6502 is not fast enough to keep up ).
//      - .sid load address must be at $0400 or greater
//      - total size of .sid file must be less then 14000 bytes (Around 95% of them are, unfortunattelly best sids are in those 5%)
//
// STM32F103C8
// This project is purely for my own entertainment , so WITHOUT ANY WARRANTY!
// I had tried to make it as much as posible to be portable for other microcontrollers
// My choise of STM32F103C8 is only because of it's price
// in bluepill's RAM, player is at 0x300, .sid file data is at 0x400 (if microcontrollers RAM is less then end of memory address of sid file, otherwise file data is in it's original memory address)
// tunes that work with it are in /music/f103 subfolder


// STM32F401CC board:
//
// next cheapest microcontroller. Much more RAM, but not full 65536 bytes. Around $F400 is last address that can be used (tunes that have higher end address will  be relocated)
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
