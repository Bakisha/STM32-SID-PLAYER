// Choose your board and upload method from menu

// it's strongly recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE).

// STM32-SID-Player : https://github.com/Bakisha/STM32-SID-PLAYER
// HVSC database: https://www.hvsc.c64.org/ 


////////////////////////////////////////////////////////////////////////////////////////////
//
//           emulator settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define TUNE_PLAY_TIME 360              // Can't implement songlenghts, manual values are needed (in seconds)//  TODO: try to determine silence in output, and skip to next tune


/////////////////////////////////////////////////////////////////////////////////////////////
//
// copy-paste tune's path here (from 01_tunes.h or 01_HVSC.h)
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include"tunes/f103/Supremacy.h" 

////////////////////////////////////////////////////////////////////////////////////////////
//
//          optional hardware settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_OUT       PA8                 // can't be changed, this is just reminder 

#define USE_SERIAL                          // for debugging info on Serial (usually USB Serial), uncomment if it's needed
#define USE_SERIAL1                         // for debugging info on Serial1 (usually on PA9/PA10), uncomment if it's needed
#define SERIAL_SPEED 9600                   // Speed of serial connection




#define BUTTON_1        PB0                 // can be any pin, but must exist. Multiple functions:
//                                             - 1 short click  - play next tune
//                                             - 2 short clicks - play next tune
//                                             - 3 short clicks - play next tune
//                                             - 4 short clicks - show HELP (on any output defined)
//                                             - 5 short clicks - show info about sid file (on any output defined)
//
//
//                                             - button holding - play tune as fast as possible (fast forward)


////////////////////////////////////////////////////////////////////////////////////////////
//
//          optional emulator settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define USE_FILTERS                        // uncomment for testing, irq is  faster in calculations (so multiplier will be smaller, and samplerate will be higher)

#define FILTER_SENSITIVITY  4              // Lower is better. Best is 1.  It will consume irq time , but FILTER_FREQUENCY can be set much higher
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
#if defined(USE_HAL_DRIVER)
#define USE_STM32duino_CORE
//                                        Official ST cores. Support for multiple line of MPU
//                                        link: https://github.com/stm32duino/Arduino_Core_STM32
//                                        Set in preferences: https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json and search stm32 in board manager.
//                                        Choose stm32 cores by ST Microelectronics. Select your CPU from tools menu


#elif defined(__STM32F1__)
#define USE_ROGER_CORE
//                                        Most of stuff for blue pill is made for this core
//                                        link: https://github.com/rogerclarkmelbourne/Arduino_STM32
//                                        Set in preferences: http://dan.drown.org/stm32duino/package_STM32duino_index.json and search stm32F1 in board manager.
//                                        Choose STM32F1xx core (NOTE: Only STM32F1 works)
#elif defined(AVR)
//                                        Must test this some day with Arduino MEGA
//
#error "Unsupported core - will try someday when i learn to setup interrupts and pwm on pins, without need to memorize TTROA, DDROB or DDWhateva"
#else
#error "Unknown or unsupported core. Maybe even both"
#endif


#include "xx_RAM.h"
#include "xx_SID.h"
#include "xx_6502.h"

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
// tunes that work with it are in /tunes/f103/ subfolder


// STM32F401CC board:
//
// next cheapest microcontroller. Much more RAM, but not full 65536 bytes. Around $F400 is last address that can be used (tunes that have higher end address will  be relocated)
// tunes that work with it are in /f401 subfolder. Ofcorse, if tune in /f103 subfolder works on f103 microcontoller, it will work on f401 microcontoller too

// STM32F411CC board:
// Next in line of cheap microcontroller boards. Full 64K of RAM for emulator. (I personally overclocked mine to 110MHz, no USB serial, but Serial on PA9/PA10 works fine)






//SCHEMATICS (not to scale) :
//
//STM32F103C8/B - STM32F401CC - STM32F411CE :
//
//  .-------------------------------------.
//  |                                     |
//  | STM32FxxxXXxx                       |
//  .---------------------------|-----|---.
//  |G                          P    P|
//  |N                          B    A|
//  |D                          0    8-----|R1|------|C2|----------|
//  |                           |                |                 --
//  |                           |.               C                 || P1
//  |                            / SW            1                 ||<----------------| OUDIO OUT
//  |                           |                |                 --
//  .---------------------------|----------------|-----------------|------------------| GND
//                                             GND
//
//
//  STM32F407VET6 black board   :
//
//  .-------------------------------------.
//  |                                     |
//  | STM32F407VE                         |
//  .---------------------------|-----|---.
//  |G                          P    P|
//  |N                          E    A|
//  |D                          3    8-----|R1|------|C2|----------|
//  |                           |                |                 --
//  |                           |.               C                 || P1
//  |                            / SW            1                 ||<----------------| OUDIO OUT
//  |                           |                |                 --
//  .---------------------------|----------------|-----------------|------------------| GND
//                                             GND
//  R1 = 100-500 Ohm
//  C1 = 10-100 nF
//  C2 = 10 uF
//  P1 = 10KOhm potentiometer
//
//
//
//
//HAVE FUN :-)
