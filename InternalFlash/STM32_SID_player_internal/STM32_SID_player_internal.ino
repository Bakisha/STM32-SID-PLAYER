
//#define USE_CORE_STM32_ST // uncomment if using this core. Set in preferences: https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json and search stm32 in board manager. Choose stm32 cores by ST Microelectronics. Select your CPU from tools menu)
#define USE_MAPLE_CORE // // uncomment if using this core. Set in preferences: http://dan.drown.org/stm32duino/package_STM32duino_index.json and search stm32F1 in board manager. Choose STM32F1xx core (NOTE: Only STM32F1 works)

// just make sure right board is selected  from board menu or you'll have compiling errors
// it's recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE). (for some reasons LTO optimatization and Newlib Standard  doesn't work)
// Can play only single speed irq based sids (no digis, sry. emulator is instruction based,not cycle exact based, and can't reach 1MHz speed). But it is fast enough to do irq instructions untill next frame.

#define USE_FILTERS // uncomment for testing, irq is  faster in calculations 
#define FILTER_FREQUENCY 12500 // maximum is 18000 for FILTER_SENSITIVITY 1 , lower it if increasing FILTER_SENSITIVITY. You'll know it if it's very high (it will totaly ruin volume)
#define FILTER_SENSITIVITY  2// Lower is better. Best is 1 . It will consume irq time (<multiplier> must be higher), but FILTER_FREQUENCY can be set much higher. For value 8, maximum FILTER_FREQUENCY is 8000. For value 16, it's 4000. For 4 and 2, it's 12500
// play with these two values to determine what is "SID-like" sound
// NOTE: for now, filters are linear in all frequencies range

#define RAM_SIZE 0x3600 + 0x400  // Set the amount of RAM that can be available to emulator (leave at least 2000 bytes for locals) (depends of microcontroller's RAM and core used)

#define USE_CHANNEL_1 // uncomment, for testing (affect only volume, not the calculations)
#define USE_CHANNEL_2 // (but it will have effect on filter output in case of multiple voices is filtered)
#define USE_CHANNEL_3 // 

#define TUNE_PLAY_TIME 180 // Can't implement songlenghts, manual values are needed (in seconds) // TODO: add buttons to change subtunes // TODO2: try to determine silence in output, and skip to next tune
uint8_t DEFAULT_SONG = 0; // 0 is automatic, from sid header, any other value is tune number

const uint8_t period = 8;// period for timer1, for frequency and resolution of PWM in uS
// can be changed, lowest is 1. Must be less or equal of variable "multiplier". Can be squeeky.
// value of 1 represent number of cpu cycles in 1 uS. (cpu_speed * period) is PWM resolution .
// Best to set as multiplier, then lower it untill squeeks are gone
// If set 1, it could be used even without external low-pass filter

const uint8_t multiplier = 40 ; // (byte) set this for interrupt speed in uS (in general, how much slower then real SID). Don't set too small, irq might overlap (music will play  slower or won't play at  all)
// needed for Timer2 (it also affect calculations in frequency multiplications per irq- it may affect tunes that uses Test-bit).
// Ideally, this should be 1 (to cycle-exact emulate SID), but irq will need to respond and exit in next 500nS
// Not with Bluepill, but for 2$ board, i'll make what i can
// Minimum is 1 (as a number, don't set iit, aas iiiit wiiiiiiil beeeeeee sloooooooooooooow) (but it is fun to see how bluepill cycle emulate SID :-) )
// Maximum is 248 , but that will greatly lower quality of high frequency sounds (Pulse voices might not even work, noise will be on lower frequencies, etc...) But please do try it :-D
// Based on current code, optimal timing is:
// 32 (+/-4) for normal (72Mhz) speed
// You could say that SAMPLE_RATE=1000000/multiplier
// Sound is not buffered, volume is calculated every <multiplier> uS

#include "tunes.h" // pick only one
// #include "HVSC.h" // better not to include it, easier navigation when copied few lines from "HVSC.h" to "tunes.h" 
#include "SID.h"  // too much of them, looks nicer when separated in it's own .h file
#include "6502.h"


void setup() {
  Init6502(); // Emulator/player initialization (located in inside 6502.ino )
  InitHardware(); // Setup timers and interrupts 

} //

void loop() {// One 6502 instruction per loop
  // Player routine
  // pritty much barebone
  PB12_HIGH;
  exec6502(); // execute 1 instruction
  //    while (STAD4XX == 1) { // was $d404,$d40b,$d412,$d418 accessed?
  //   } // it can be left out if <multiplier> is small enough, so no more then 4 or 5 6502 instruction is executed between irq's
  if (JSR1003 == 1) { // JSR1003 check
    JSR1003 = 0;
    while (VIC_irq_request == 0) { // wait untill irq say it's new frame
    }
    VIC_irq_request = 0;
  }

  PB12_LOW; // test point - function overhead

} // loop



// INFO:

//      - only music based on raster irq (play address must not be 0, will not work with custom music player inside .sid file)
//      - multi-speed .sid files are not supported, ( emulation of 6502 is not fast enough to keep up ).
//      - .sid load address must be at $0400 or greater
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




// SCHEMATICS:
//
//
//    .-----------------.
//    |                 |
//    | STM32FxxxXXxx   |
//    .------------|----.
//     |G         P|
//     |N         A|
//     |D         8--R1----|------C2---------> OUDIO OUT
//     |                   C
//     |                   1
//     .-------------------|------------------ GND
//                        GND
//    R1 = 100-500 Ohm
//    C1 = 100 nF
//    C2 = 10 uF
//
// If <period> is 1 , AUDIO OUT can be connected to PA8 (no need for R1,C1 and maybe C2). I don't think 1Mhz sample rate will be in hearing range



// TODO :
// Read .sid files from SD Card and copy it to RAM if it's less then available RAM
// For now every .sid file is converted to byte array in /tunes subfolder , and added as #include "name_of_the_file.h" in SID.h
