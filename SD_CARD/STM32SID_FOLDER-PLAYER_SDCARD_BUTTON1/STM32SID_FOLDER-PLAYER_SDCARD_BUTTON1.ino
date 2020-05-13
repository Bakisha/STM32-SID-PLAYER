// Choose your board and upload method from menu  (current setttings is for BluePill - STM32F103C8, ROGER's or STM32Duino core )
// it's strongly recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE).
//
//                                                  STM32-SID-Player:     https://github.com/Bakisha/STM32-SID-PLAYER
//                                                  HVSC database:        https://www.hvsc.c64.org/downloads (download and unpack to SD Card)

/////////////////////////////////////////////////////////////////////////////////////////////
//
//           Player configuration:
//
//////////////////////////////////////////////////////////////////////////////////////////////


#define TUNE_PLAY_TIME 215                           // Can't implement songlenghts, manual values are needed (in seconds)//  TODO: try to determine silence in output, and skip to next tune

const char * const HVSC = "HVSC" ;                   // ---> IMPORTANT! <---  name of HVSC SID Collection folder on your SD Card ("HVSC", "C64Music", "/" or "" for root, etc"

//#include "01_HVSC.h"                               // disabled for Bluepill. Can be enabled, but must be compiled with O0 optimatization (which will decrease sound quality)
//                                                    When Random play is enabled, must manual switch between playlists


#define NUMBER_OF_FAVORITE_FOLDERS 31                // set number of folder for playlist. Must have at least 1.


const char * const HVSC_FAVORITES                    //  set favorite directories paths (relative to main HVSC folder) with sid files in it
[] =
{
  "MUSICIANS/G/Gregfeel/",
  "MUSICIANS/B/Blues_Muz/Gallefoss_Glenn/",
  "FAVORITES/",
  "MUSICIANS/L/Linus/",
  "MUSICIANS/P/Page_Jason/", // lot of multi-speed tunes (CIA speed test)
  "MUSICIANS/D/Dunn_David/",
  "MUSICIANS/T/Tel_Jeroen/",
  "MUSICIANS/R/Rowlands_Steve/",
  "MUSICIANS/V/Vincenzo/",
  "MUSICIANS/D/Daglish_Ben/",
  "MUSICIANS/G/Gray_Matt/",
  "MUSICIANS/G/Gas_On/",
  "MUSICIANS/B/Blues_Muz/" ,
  "MUSICIANS/S/Sequencer/",
  "DEMOS/G-L/",
  "MUSICIANS/G/Gray_Matt/",
  "MUSICIANS/F/Fanta/",
  "MUSICIANS/T/The_Syndrom/",
  "MUSICIANS/L/LMan/",
  "MUSICIANS/L/Laxity/",
  "MUSICIANS/K/KB/",
  "MUSICIANS/H/Hesford_Paul/",
  "MUSICIANS/G/Goto80/",
  "MUSICIANS/M/Mahoney/",
  "MUSICIANS/H/Hubbard_Rob/",
  "MUSICIANS/B/Blues_Muz/",
  "MUSICIANS/P/PVCF/",
  "MUSICIANS/J/JCH/",
  "MUSICIANS/R/Rowlands_Steve/",
  "MUSICIANS/H/Huelsbeck_Chris/",
  "MUSICIANS/G/Galway_Martin/"

};

bool favorites_finished = false;                  // - initial value. If "HVSC.h" is included, "true" will play it, otherwise, favorite folders will play first.
bool RANDOM_FOLDERS = false;                      // - play folders playlist in alphabetical or random order (note: when enabled, switching to other folder playlist is only possible with button)

////////////////////////////////////////////////////////////////////////////////////////////
//
//          hardware settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_OUT       PA8                 // can't be changed, this is just reminder 

#define USE_SERIAL                          // for debugging info on Serial (usually USB Serial), uncomment if it's needed
//#define USE_SERIAL1                         // for debugging info on Serial1 (usually on PA9/PA10), uncomment if it's needed
#define SERIAL_SPEED 9600                   // Speed of serial connection

#define CS_SDCARD       PA1                 // can be changed
#define SD_SPEED        20                   // SD Card SPI speed in MHz

#define BUTTON_1        PB0                 // can be any pin, but must exist. Multiple functions:
//                                             - 1 short click  - play next tune
//                                             - 2 short clicks - play next file
//                                             - 3 short clicks - play next folder
//                                             - 4 short clicks - show HELP (on any output defined)
//                                             - 5 short clicks - show info about sid file (on any output defined)
//                                             - 6 short clicks - switch FAVORITE/ALL folder playlist
//                                             - 7 short clicks - switch random play ON/OFF
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


/*


  SID chip (6581 and 8580) and 6502 CPU emulator, with audio output on pin PA8, for STM32 line of microcontrollers, compiled with Arduino IDE, uploaded with ST-LINK V2.

  My personal project that i was having fun for some time now. Mainly done for STM32F103C8 (Blue Pill). Should work with other STM32 microcontrollers that Arduino IDE support. Tested at STM32F401CCU6, STM32F411CEU6 and STM32F407VET6 dev boards (and as i see, they all have same pin (PA8) for Timer1,channel1), but not all have same pins for SPI.

  -Only single speed IRQ based sids can be played (PSID V2 sids, no digis, emulator is not fast enough).

  -Maximum size of sid is limited by microcontrollers RAM

  -Sid load address must be at $0400 or greater.

  -Filter emulation is far from real SID chip, but, for me, it sound enough "SID-ish".

  -Plays sid from hardcoded folders from SD CARD. Use button to switch to next tune/file/folder.

  -SID tune database can be found at  https://www.hvsc.c64.org/

  -It is configurable. On first tab in Arduino IDE is values that can be changed to match microcontroller used. Currently it's set for STM32F103C8.

  I'm not good at C, so code is maybe a mess, but it works :-) Code used for emulator is found at http://rubbermallet.org/fake6502.c

  reSID can be found at https://en.wikipedia.org/wiki/ReSID. This is not reSID port, but i did used some parts of reSID code.


SCHEMATICS (not to scale) :

STM32F103C8/B - STM32F401CC - STM32F411CE :

  .-------------------------------------.
  |                                     |
  | STM32FxxxXXxx                       |
  .---------------------------|-----|---.
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
  .---------------------------|-----|---.
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

*/
