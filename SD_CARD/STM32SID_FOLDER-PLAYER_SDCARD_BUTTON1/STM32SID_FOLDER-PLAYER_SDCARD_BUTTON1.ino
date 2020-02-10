// Choose your board and upload method from menu
// Choose available RAM for emulator (depending of microcontroller) (currently set for BluePill - STM32F103C8, any core )
// it's strongly recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE).

////////////////////////////////////////////////////////////////////////////////////////////
//
//           emulator settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define RAM_SIZE 0x3000                // set this value based on microcontroller used. maximum is 65535 bytes ( 0xFFFF HEX ) or available microcontoller's RAM
#define TUNE_PLAY_TIME 180                // Can't implement songlenghts, manual values are needed (in seconds)//  TODO: try to determine silence in output, and skip to next tune
uint8_t DEFAULT_SONG = 1;               // 0 is automatic, from sid header, any other value is tune number


/////////////////////////////////////////////////////////////////////////////////////////////
//
// HVSC directory
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define NUMBER_OF_FOLDERS 27

// set directories paths with sid files in it (  // must start and end with "/" )
// press CTRL-F to replace path of HVSC folder path on your SD card
const char * HVSC_DIR [NUMBER_OF_FOLDERS] = {
  "/HVSC/MUSICIANS/B/Blues_Muz/" ,
  "/HVSC/MUSICIANS/S/Sequencer/",
  "/HVSC/DEMOS/G-L/",
  "/HVSC/MUSICIANS/G/Gray_Matt/",
  "/HVSC/MUSICIANS/D/Daglish_Ben/",
  "/HVSC/MUSICIANS/R/Rowlands_Steve/",
  "/HVSC/MUSICIANS/F/Fanta/",
  "/HVSC/MUSICIANS/T/The_Syndrom/",
  "/HVSC/MUSICIANS/G/Gas_On/",
  "/HVSC/MUSICIANS/T/Tel_Jeroen/",
  "/HVSC/MUSICIANS/L/LMan/",
  "/HVSC/MUSICIANS/L/Laxity/",
  "/HVSC/MUSICIANS/K/KB/",
  "/HVSC/MUSICIANS/H/Hesford_Paul/",
  "/HVSC/MUSICIANS/G/Goto80/",
  "/HVSC/MUSICIANS/M/Mahoney/",
  "/HVSC/MUSICIANS/H/Hubbard_Rob/",
  "/HVSC/MUSICIANS/B/Blues_Muz/",
  "/HVSC/MUSICIANS/B/Blues_Muz/Gallefoss_Glenn/",
  "/HVSC/MUSICIANS/P/PVCF/",
  "/HVSC/MUSICIANS/G/Gregfeel/", // 1st "random"
  "/HVSC/MUSICIANS/J/JCH/",
  "/HVSC/MUSICIANS/R/Rowlands_Steve/",
  "/HVSC/MUSICIANS/H/Huelsbeck_Chris/",
  "/HVSC/MUSICIANS/G/Galway_Martin/",
  "/HVSC/MUSICIANS/V/Vincenzo/",
  "/"
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//          hardware settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define USE_SERIAL                                // for debugging info on Serial , uncomment if it's needed
#define USE_SERIAL1                               // for debugging info on Serial1, uncomment if it's needed

#define BUTTON_1        PB0                       // can be any pin , work without button, it will skip to next tune when timed out

#define CS_SDCARD       PA1                       // can be changed
#define SD_CLK          PA5
#define SD_MISO         PA6
#define SD_MOSI         PA7

#define AUDIO_OUT       PA8                        // can't be changed, this is just reminder

#include <SPI.h>
#include <SdFat.h>

SdFat sd;
SdFile root;
SdFile sidfile;
SdFile nextfile;


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

#ifdef USE_HAL_DRIVER // Official ST cores. Support for multiple line of MPU
#define USE_STM32duino_CORE //  Set in preferences: https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json and search stm32 in board manager. Choose stm32 cores by ST Microelectronics. Select your CPU from tools menu)
#else
#define USE_ROGER_CORE //  Set in preferences: http://dan.drown.org/stm32duino/package_STM32duino_index.json and search stm32F1 in board manager. Choose STM32F1xx core (NOTE: Only STM32F1 works)
#endif

#include "SID.h"
#include "6502.h"


/*


  SID chip (6581 and 8580) and 6502 CPU emulator, with audio output on pin PA8, for STM32 line of microcontrollers, compiled with Arduino IDE, uploaded with ST-LINK V2.

  My personal project that i was having fun for some time now. Mainly done for STM32F103C8 (Blue Pill). Should work with other STM32 microcontrollers that Arduino IDE support. Tested also at STM32F401CCU6 dev board (and as i see, they all have same pin (PA8) for Timer1,channel1) and same pins for SPI.

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
  R1 = 100-500 Ohm
  C1 = 100 nF
  C2 = 10 uF
  P1 = 10KOhm potentiometer

  If period is 1 , there is no need for R1 and C1.

  Make sure right board is selected in Arduino IDE, and uncomment line depending of what core from board manager is used.

  HAVE FUN :-)

*/
