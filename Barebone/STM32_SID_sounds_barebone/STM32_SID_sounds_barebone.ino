
// SID emulation only
// I use it as base for my projects that need simple sounds or sound effects.

//#define USE_DAC                             // if defined, use DAC output on pin PA4, if commented out, use PWM on pin PA8 

#include "SID.h"

// INFO:


// This project is purely for my own entertainment , so WITHOUT ANY WARRANTY!
// SCHEMATICS:
//
// PWM: 
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
/*
  .------------------------------------------------------.
  |                                                      |
  | STM32G31CBU6                                         |
  .--------------+---------------------------|-----------.
  |G            P|                                                
  |N            A|
  |D            4.----------------------|
  |                                     --
  |                                     || P1
  |                                     ||<------|C|-------| OUDIO OUT
  |                                     --
  .-------------------------------------|------------------| GND
  C = 10 uF
  P1 = 10KOhm potentiometer

*/



void setup() {




  // list of global variables that control SID emulator . Keep in mind that IRQ will detect change after some delay (around 50uS)
  // detailed information: http://archive.6502.org/datasheets/mos_6581_sid.pdf

  // channel 1
  OSC_1_HiLo            = 0;              // 0-65535      //
  PW_HiLo_voice_1       = 0;              // 0-4095       //
  noise_bit_voice_1     = 0;              // true/false   //
  pulse_bit_voice_1     = 0;              // true/false   //
  sawtooth_bit_voice_1  = 0;              // true/false   //
  triangle_bit_voice_1  = 0;              // true/false   //
  test_bit_voice_1      = 0;              // true/false   //
  ring_bit_voice_1      = 0;              // true/false   //
  SYNC_bit_voice_1      = 0;              // true/false   //
  Gate_bit_1            = 0;              // true/false   //
  ADSR_Attack_1         = 0;              // 0-15         //
  ADSR_Decay_1          = 0;              // 0-15         //
  ADSR_Sustain_1        = 0;              // 0-15         //
  ADSR_Release_1        = 0;              // 0-15         //

  // channel 2

  OSC_2_HiLo            = 0;              // 0-65535      //
  PW_HiLo_voice_2       = 0;              // 0-4095       //
  noise_bit_voice_2     = 0;              // true/false   //
  pulse_bit_voice_2     = 0;              // true/false   //
  sawtooth_bit_voice_2  = 0;              // true/false   //
  triangle_bit_voice_2  = 0;              // true/false   //
  test_bit_voice_2      = 0;              // true/false   //
  ring_bit_voice_2      = 0;              // true/false   //
  SYNC_bit_voice_2      = 0;              // true/false   //
  Gate_bit_2            = 0;              // true/false   //
  ADSR_Attack_2         = 0;              // 0-15         //
  ADSR_Decay_2          = 0;              // 0-15         //
  ADSR_Sustain_2        = 0;              // 0-15         //
  ADSR_Release_2        = 0;              // 0-15         //

  // channel 3
  OSC_3_HiLo            = 0;              // 0-65535      //
  PW_HiLo_voice_3       = 0;              // 0-4095       //
  noise_bit_voice_3     = 0;              // true/false   //
  pulse_bit_voice_3     = 0;              // true/false   //
  sawtooth_bit_voice_3  = 0;              // true/false   //
  triangle_bit_voice_3  = 0;              // true/false   //
  test_bit_voice_3      = 0;              // true/false   //
  ring_bit_voice_3      = 0;              // true/false   //
  SYNC_bit_voice_3      = 0;              // true/false   //
  Gate_bit_3            = 0;              // true/false   //
  ADSR_Attack_3         = 0;              // 0-15         //
  ADSR_Decay_3          = 0;              // 0-15         //
  ADSR_Sustain_3        = 0;              // 0-15         //
  ADSR_Release_3        = 0;              // 0-15         //

  // other registers
  FILTER_HiLo           = 0;              // 0-2047       //
  FILTER_Resonance      = 0;              // 0-15         //
  FILTER_Enable_1       = 0;              // true/false   //
  FILTER_Enable_2       = 0;              // true/false   //
  FILTER_Enable_3       = 0;              // true/false   //
  OFF3                  = 0;              // true/false   //
  FILTER_HP             = 0;              // true/false   //
  FILTER_BP             = 0;              // true/false   //
  FILTER_LP             = 0;              // true/false   //
  MASTER_VOLUME         = 0;              // 0-15         //

  InitHardware(); // 2_setup.ino  (start SID emulator)

  reset_SID(); // in 6_barebone_sounds.ino
}


void loop() {

  // sounds in 6_barebone_sounds.ino (names are just from another projects)
  error_sound_SD();
  delay(1000);
  error_sound_ROOT();
  delay(1000);
  error_open_file();
  delay(1000);
  error_open_folder ();
  delay(1000);
  error_open_sid ();
  delay(1000);
  error_PSID_V2_RAM_OVERFLOW();
  delay(1000);
}
