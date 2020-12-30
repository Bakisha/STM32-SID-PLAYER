#ifndef _whatAmess
#define _whatAmess

//////////////////////////////////////////////////////////////////////////////////////////////
//
// initial defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

// Choose your board and upload method from menu

// it's strongly recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE).

// STM32-SID-Player : https://github.com/Bakisha/STM32-SID-PLAYER
//


////////////////////////////////////////////////////////////////////////////////////////////
//
//           emulator settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define TUNE_PLAY_TIME 350              // Can't implement songlenghts, manual values are needed (in seconds)//  TODO: try to determine silence in output, and skip to next tune



////////////////////////////////////////////////////////////////////////////////////////////
//
//          optional hardware settings
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_OUT       PA8                 // can't be changed, this is just reminder 

//#define USE_SERIAL                          // for debugging info on Serial (usually USB Serial), uncomment if it's needed
//#define USE_SERIAL1                         // for debugging info on Serial1 (usually on PA9/PA10), uncomment if it's needed
#define SERIAL_SPEED 9600                   // Speed of serial connection




//#define BUTTON_1        PB0                 // can be any pin, but must exist. Multiple functions:
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

#define FILTER_SENSITIVITY  16              // Lower is better. Best is 1.  It will consume irq time , but FILTER_FREQUENCY can be set much higher
#define FILTER_FREQUENCY 4000             // You'll know it if it's very high (it will totaly ruin sound)

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




//////////////////////////////////////////////////////////////////////////////////////////////
//
// #include "xx_RAM.h"
//
//////////////////////////////////////////////////////////////////////////////////////////////

//  from SDfat library
extern "C" char* sbrk(int incr);
// free RAM (actually, free stack
inline uint32_t FreeBytes() {
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));
}




#if defined(SPI_RAM)
// TODO
#elif defined(PARALLEL_SRAM)
// TODO
#else
uint16_t RAM_SIZE = 0;
uint8_t * RAM = NULL;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// #include "xx_SID.h"
//
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _sid
#define _sid
uint8_t period = 4;//                        period for timer1, for frequency and resolution of PWM in uS
//                                           Automatic config set this as 4, if calculated multiplier is greater then 12 (uS), otherwise, is same as multiplier.
//                                           value of 1 represent number of cpu cycles in 1 uS. (cpu_speed * period) is PWM resolution .

uint8_t multiplier  ;//
//                                           (byte) Interrupt speed in uS (in general, how much slower then real SID). Automatic config will search for value that has SID emulator run under 13mS per frame.
//                                           needed for Timer2 (it also affect calculations in frequency multiplications per irq- it may affect tunes that uses Test-bit).
//                                           Ideally, this should be 1 (to cycle-exact emulate SID), but irq will need to respond and exit in next 500nS
//                                           Not with Bluepill, but for 2$ board, i'll make what i can
//                                           Minimum is 1 (as a number, if set manualy , wiiiiiiil beeeeeee sloooooooooooooow) (but it is fun to see how bluepill cycle emulate SID :-) )
//                                           Maximum is 248 , but that will greatly lower quality of high frequency sounds (Pulse voices might not even work, noise will be on lower frequencies, etc...)
//                                           You could say that SAMPLE_RATE=1000000/multiplier
//                                           Sound is not buffered, volume is calculated every <multiplier> uS
//



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t magic_number = F_CPU / 1000000 ; // PWM resolution - number of cycles in 1 uS (PWM resolution = period * magic_number) // keep as same as speed of microcontroller, in Mhz
// at 72MHz there is 72 clock "ticks" in 1uS that drives counter of Timer1. It works with 128MHZ bluepill overclocked. It also works underclocked at 48MHz.
// STM32duino boards have this as CYCLES_PER_MICROSECOND
// STM32 boards have this as (F_CPU / 1000000)
// TODO: It's a 8bit number, so maximum clock is 255. TODO: See if it brake stuff if it's 16bit number.

//uint32_t SID_data_size = 0;

uint8_t previous_Tune = Tune;

uint8_t NUMBER_OF_INSTRUCTION_PER_IRQ = 3;

volatile bool SIDplaying = true; // play tune as soon as initalization is complete

uint32_t Total_IRQ_uS = 0;

uint8_t  CIA_DC04 = 0;
uint8_t  CIA_DC05 = 0;

uint16_t MagicID = 0;
uint16_t VERSION = 0;
bool RAM_OVERFLOW = 0;

bool PLAYABLE_SID = false;
//bool autoconfigFinished = false;
//bool JAMMED = false;

//char SID_filename[128] ; // reserve 128 bytes for file name
//char SID_DIR_name [128] ; // reserve 128 bytes for directory path name
//char SID_path_filename [256] ; // reserve 256 bytes for full path of file
//bool IS_SID_FILE = false; //  for extension check
//int16_t current_folder = -1 ; // so first load is set to 0
//

volatile bool next_tune = false;
//volatile bool play_next_folder = true;
//volatile bool try_again = true;
//volatile bool load_next_file = true;

uint32_t IRQ_TYPE_PER_TUNE = 0; // subtune speed info
uint32_t VIDEO_TYPE = 0; // pal or ntsc
uint32_t MODEL_TYPE = 0; //MOS6581 or MOS8580
uint32_t FLAGS76 = 0;
uint8_t ComputeMUSplayer = 0;
uint8_t C64Compatible = 0;
uint8_t LOW_RAM_uncompatible = 0;
uint16_t LOAD_ADDRESS = 0;
uint32_t SUBTUNE_SPEED = 0;

uint32_t SID_speed = 0;//20000; // value set in 2_setup.ino(in uS)

uint16_t SID_load_start = 0; //SID_data[0x7c] + (SID_data[0x7d] * 256); // get start address from .sid file
uint16_t SID_load_end = 0; //SID_start + SID_data_size - 0x7e ; // end address of music routine , not included "busy blocks", aka, ram needed after end of actual sid file. If sid is longer then available RAM, everything above available RAM will be read-only (from SID_data[] array)

uint16_t SID_play = 0;//SID_data[13] + (SID_data[12] * 256); // sid play address
uint16_t SID_init = 0;//SID_data[11] + (SID_data[10] * 256); // sid init address

uint8_t SID_default_tune = 0;//SID_data[17] + (SID_data[16] * 256); // default song to be played first
uint8_t SID_number_of_tunes = 0;//SID_data[15] + (SID_data[14] * 256); // number of tunes in sid
uint8_t SID_current_tune = 0;//SID_default_tune;



uint32_t tune_play_counter; // uS counter
uint32_t tune_end_counter = 1000000 * TUNE_PLAY_TIME; // play new tune every x seconds  (this is the number in uS) (maximum 32bit number is around 71minutes)

uint32_t VIC_irq = 0;
volatile  uint8_t  VIC_irq_request = 0;
volatile bool play_next_tune = 0;
volatile  uint8_t JSR1003 = 0;
volatile  uint8_t STAD4XX = 0;

uint8_t skip_counter;
uint8_t skip_counter_max = 197; //every 197th jumps into irq will be skiped, to emulate 985000Hz clock (not 1MHz )

int32_t w0 = 0;
//w0 = static_cast<sound_sample>(2*pi*f0[fc]*1.048576); // f0[fc] 0-12500 ; fc 0-7ff
// w0 = 2*pi*1.048576*(fc*12500/2047)
int32_t w0_max_dt = (2 * 3.1415926535897932385 * FILTER_FREQUENCY * 1.048576); // maximum frequency that can be filtered
int32_t w0_constant_part = 2.0 * 3.1415926535897932385 * 1.048576 * FILTER_FREQUENCY   / 2048.0; // around 40.211 per 1 value of FilterHiLo for 12500 max value // TODO : make this const array of 2048 values, as uint32_t, with FILTER_FREQUENCY as array members
int32_t w0_ceil_dt = 0.0;
int32_t w0_delta_t = 0.0;

int32_t w0_ceil_1 = 0.0;
int32_t Q_1024_div = 0;

int delta_t = 0;
int delta_t_flt = 0;

int32_t Vhp = 0;
int32_t Vbp = 0;
int32_t Vlp = 0;

int32_t dVbp = 0;
int32_t  dVlp = 0;
int32_t  dVhp = 0;

int32_t Volume_unfiltered = 0;
int32_t Volume_filtered = 0;
int32_t Volume_filter_input = 0;
int32_t Volume_filter_output = 0;

uint32_t temporary_variable_i     ;

uint8_t SID[] = {                  //  array that hold values of SID registers
  0,      // Freq_1_Lo                      - REG_0
  0,      // Freq_1_HI                      - REG_1
  0,      // PW1_Lo                         - REG_2
  0,      // PW1_Hi                         - REG_3
  0,      // ControlRegister_1              - REG_4
  // bit 0 - Gate_bit_1
  // bit 1 - Sync_bit_1
  // bit 2 - Ring_bit_1
  // bit 3 - Test_bit_1
  // bit 4 - triangle_bit_voice_1
  // bit 5 - sawtooth_bit_voice_1
  // bit 6 - pulse_bit_voice_1
  // bit 7 - noise_bit_voice_1
  0,     // Attack-Decay_1                  - REG_5
  // bit 0 - 3  // Decay_1  // 0-15
  // bit 4 - 7  // Attack_1  // 0-15
  0,      // Sustain-Release_1              - REG_6
  // bit 0 - 3  // Release_1 // 0-15
  // bit 4 - 7  // Sustain_1 // 0-15
  0,      // Freq_2_Lo                      - REG_7
  0,      // Freq_2_HI                      - REG_8
  0,      // PW2_Lo                         - REG_9
  0,      // PW2_Hi                         - REG_10
  0,      // ControlRegister_2              - REG_11
  // bit 0 - Gate_bit_2
  // bit 1 - Sync_bit_2
  // bit 2 - Ring_bit_2
  // bit 3 - Test_bit_2
  // bit 4 - triangle_bit_voice_2
  // bit 5 - sawtooth_bit_voice_2
  // bit 6 - pulse_bit_voice_2
  // bit 7 - noise_bit_voice_2
  0,     // Attack-Decay_2                  - REG_12
  // bit 0 - 3  // Decay_2 // 0-15
  // bit 4 - 7  // Attack_2  // 0-15
  0,      // Sustain-Release_2              - REG_13
  // bit 0 - 3  // Release_2 // 0-15
  // bit 4 - 7  // Sustain_2 // 0-15
  0,      // Freq_3_Lo                      - REG_14
  0,      // Freq_3_HI                      - REG_15
  0,      // PW3_Lo                         - REG_16
  0,      // PW3_Hi                         - REG_17
  0,      // ControlRegister_3              - REG_18
  // bit 0 - Gate_bit_3
  // bit 1 - Sync_bit_3
  // bit 2 - Ring_bit_3
  // bit 3 - Test_bit_3
  // bit 4 - triangle_bit_voice_3
  // bit 5 - sawtooth_bit_voice_3
  // bit 6 - pulse_bit_voice_3
  // bit 7 - noise_bit_voice_3
  0,     // Attack-Decay_3                  - REG_19
  // bit 0 - 3  // Decay_3  // 0-15
  // bit 4 - 7  // Attack_3  // 0-15
  0,      // Sustain-Release_3              - REG_20
  // bit 0 - 3  // Release_3 // 0-15
  // bit 4 - 7  // Sustain_3 // 0-15
  0,    // FILTER_Lo (0-7)                  - REG_21
  // bit 0 - 2
  0,    // FILTER_Hi ( 0 - 255 )            - REG_22
  0,     // RES_FILT                        - REG_23
  // bit 0    FILTER_Enable_1
  // bit 1    FILTER_Enable_2
  // bit 2    FILTER_Enable_3
  // bit 3    FILTER_Enable_EXT
  // bit 4 - 7  // Resonance  // 0-15
  0,    // FILTER_MODE-MASTER_VOLUME        - REG_24
  // bit 0 - 3  // MASTER_VOLUME  // 0-15
  // bit 4    FILTER_LP
  // bit 5    FILTER_BP
  // bit 6    FILTER_HP
  // bit 7    OFF3
  0,    // POTX                             - REG_25
  0,    // POTY                             - REG_26
  0,    // OSC3                             - REG_27
  0     // ENV3                             - REG_27

};



// BlueSID



uint8_t MASTER_VOLUME = 15; // $D418 //


uint16_t FILTER_HiLo = 0; // 11bit
uint8_t FILTER_Resonance = 0; // 4bit
uint8_t FILTER_Enable_1 = 0; // on/off
uint8_t FILTER_Enable_2 = 0; // on/off
uint8_t FILTER_Enable_3 = 0; // on/off
uint8_t FILTER_Enable_switch = 0; // Filter_Enable_123 in one variable
uint8_t FILTER_LP = 0; // on/off
uint8_t FILTER_HP = 0; // on/off
uint8_t FILTER_BP = 0; // on/off
uint8_t FILTER_Enable_EXT = 0; // on/off


uint8_t OSC3 = 0; // Register 27 - Oscilator output

uint8_t OFF3 = 0; //Register 24, bit 7 . Disable voice 3 output
uint8_t ENV3 = 0; // $D428
uint8_t POTX = 0; // TODO
uint8_t POTY = 0; // TODO



uint8_t SYNC_bit_voice_1 = 0;
uint8_t SYNC_bit_voice_2 = 0;
uint8_t SYNC_bit_voice_3 = 0;

uint8_t  test_bit_voice_1 = 0;
uint8_t  test_bit_voice_2 = 0;
uint8_t  test_bit_voice_3 = 0;

uint8_t ring_bit_voice_1 = 0;
uint8_t ring_bit_voice_2 = 0;
uint8_t ring_bit_voice_3 = 0;


uint16_t main_volume = 0;
uint32_t main_volume_32bit = 0;

/////////////////////////////// signed variables

int32_t Volume = 0;


int32_t Volume_1 = 0;
int32_t Volume_2 = 0;
int32_t Volume_3 = 0;


////////////////////////////////////////


// main OSCilator variables
uint32_t OSC_1 = 0; // main 24bit  accumulator oscilator - voice 1
uint32_t OSC_2 = 0; // main 24bit  accumulator oscilator - voice 2
uint32_t OSC_3 = 0; // main 24bit  accumulator oscilator - voice 3


uint32_t OSC_1_HiLo = 0x0000; // 0
uint32_t OSC_2_HiLo = 0x0000; // 0
uint32_t OSC_3_HiLo = 0x0000; // 0



// constants
const uint32_t max_OSC = 16777216;   // B0001 0000 0000 0000 0000 0000 0000
const uint32_t OSC_MSB = 2048;       // B0000 0000 0000 0000 1000 0000 0000
const uint16_t B4095 = 4095;         // B0000 0000 0000 0000 1111 1111 1111
const uint16_t B4096 = 4096;         // B0000 0000 0000 0001 0000 0000 0000
const uint16_t B2047 = 2047;         // B0000 0000 0000 0000 0111 1111 1111


uint8_t OSC_MSB_1; // Oscilator's MSB
uint8_t OSC_MSB_2;
uint8_t OSC_MSB_3;

uint8_t OSC_MSB_Previous_1 = 0;
uint8_t OSC_MSB_Previous_2 = 0;
uint8_t OSC_MSB_Previous_3 = 0;

uint8_t MSB_Rising_1 = 0;
uint8_t MSB_Rising_2 = 0;
uint8_t MSB_Rising_3 = 0;

uint32_t OSC_bit19_1 = 0;
uint32_t OSC_bit19_2 = 0;
uint32_t OSC_bit19_3 = 0;

uint32_t OSC_noise_1 = 0;
uint32_t OSC_noise_2 = 0;
uint32_t OSC_noise_3 = 0;

uint8_t OSC_bit19_Previous_1 = 0;
uint8_t OSC_bit19_Previous_2 = 0;
uint8_t OSC_bit19_Previous_3 = 0;

uint32_t temp11; // upper 12 bits of OSC_1

uint32_t temp12;

uint32_t temp13;

// internal SID registers

// ADSR

// values for attack and decay/release

const uint32_t  ADSR_LFSR15 [] = {
  // step of 1 that ADSR is changing. In total it's 256 steps (from 0 to max volume) to reach time to match Envelope timing in real SID.


  /* // from reSID
        9,  //   2ms*1.0MHz/256 =     7.81
       32,  //   8ms*1.0MHz/256 =    31.25
       63,  //  16ms*1.0MHz/256 =    62.50
       95,  //  24ms*1.0MHz/256 =    93.75
      149,  //  38ms*1.0MHz/256 =   148.44
      220,  //  56ms*1.0MHz/256 =   218.75
      267,  //  68ms*1.0MHz/256 =   265.63
      313,  //  80ms*1.0MHz/256 =   312.50
      392,  // 100ms*1.0MHz/256 =   390.63
      977,  // 250ms*1.0MHz/256 =   976.56
     1954,  // 500ms*1.0MHz/256 =  1953.13
     3126,  // 800ms*1.0MHz/256 =  3125.00
     3907,  //   1 s*1.0MHz/256 =  3906.25
    11720,  //   3 s*1.0MHz/256 = 11718.75
    19532,  //   5 s*1.0MHz/256 = 19531.25
    31251   //   8 s*1.0MHz/256 = 31250.00
  */
  // from reSID, but lowered by 1
  8,  //   2ms*1.0MHz/256 =     7.81
  31,  //   8ms*1.0MHz/256 =    31.25
  62,  //  16ms*1.0MHz/256 =    62.50
  94,  //  24ms*1.0MHz/256 =    93.75
  148,  //  38ms*1.0MHz/256 =   148.44
  219,  //  56ms*1.0MHz/256 =   218.75
  266,  //  68ms*1.0MHz/256 =   265.63
  312,  //  80ms*1.0MHz/256 =   312.50
  391,  // 100ms*1.0MHz/256 =   390.63
  976,  // 250ms*1.0MHz/256 =   976.56
  1953,  // 500ms*1.0MHz/256 =  1953.13
  3125,  // 800ms*1.0MHz/256 =  3125.00
  3906,  //   1 s*1.0MHz/256 =  3906.25
  11719,  //   3 s*1.0MHz/256 = 11718.75
  19531,  //   5 s*1.0MHz/256 = 19531.25
  31250   //   8 s*1.0MHz/256 = 31250.00

};
// value is 1 per 1uS// / 256 is becasue there are 256 values of ADSR volume
// 2ms,8ms,16ms,24ms,38ms,56ms,68ms,80ms,100ms,250ms,500ms,800ms,1s,3s,5,8s for attack
// 6ms,24ms,48ms,72ms,114ms,168ms,68ms,240ms,300ms,750ms,1500ms,2400ms,3s,9s,15,24s for decay/release



//ADSR 1

uint16_t ADSR_volume_1 = 0; // 8bit ADSR volume, but must use uint32_t because it will  // 256 different values of volume
uint32_t LFSR15_1 = 0x7fff; // 15bit ADSR LSFR //
uint32_t LFSR15_comparator_value_1 = 0; // 15bit comparator for LFSR15
uint16_t Divided_LFSR15_1 = 0; // whole number helper variable
uint16_t Divided_LFSR5_1 = 0; // whole number helper variable
uint32_t LFSR5_1 = 0; //  counter that is dividing  LFSR15 counter to ADSR_Volume counter. It is taken from predefined ADSR_Volume values (Catch22)
uint32_t LFSR5_comparator_value_1 = 1; // value that... uff... compare
uint8_t ADSR_Attack_1 = 0; // Attack value (0-15)
uint8_t ADSR_Decay_1 = 0; // Decay value (0-15)
uint8_t ADSR_Sustain_1 = 0; // Sustain value (0-15)
uint8_t ADSR_Release_1 = 0; // Release value (0-15)
uint8_t ADSR_stage_1;  // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
bool hold_zero_1 = false;

//ADSR 2

int16_t ADSR_volume_2 = 0; // 8bit ADSR volume // 256 different values of volume
uint32_t LFSR15_2 = 0x7fff; // 15bit ADSR LSFR, but can be used as counter in  coding //
uint32_t LFSR15_comparator_value_2 = 0; // 15bit comparator for LFSR15
uint16_t Divided_LFSR15_2 = 0;
uint16_t Divided_LFSR5_2 = 0;
uint32_t LFSR5_2 = 0; //  counter that is dividing  LFSR15 counter to ADSR_Volume counter. It is dirived from predefined ADSR_Volume values (Catch22)
uint32_t LFSR5_comparator_value_2 = 1; // value that
uint8_t ADSR_Attack_2 = 0; // Attack value (0-15)
uint8_t ADSR_Decay_2 = 0; // Decay value (0-15)
uint8_t ADSR_Sustain_2 = 0; // Sustain value (0-15)
uint8_t ADSR_Release_2 = 0; // Release value (0-15)
uint8_t ADSR_stage_2;  // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
bool hold_zero_2 = false;
//ADSR 3

int16_t ADSR_volume_3 = 0; // 8bit ADSR volume // 256 different values of volume
uint32_t LFSR15_3 = 0x7fff; // 15bit ADSR LSFR, but can be used as counter in  coding //
uint32_t LFSR15_comparator_value_3 = 0; // 15bit comparator for LFSR15
uint16_t Divided_LFSR15_3 = 0;
uint16_t Divided_LFSR5_3 = 0;
uint32_t LFSR5_3 = 0; //  counter that is dividing  LFSR15 counter to ADSR_Volume counter. It is dirived from predefined ADSR_Volume values (Catch22)
uint32_t LFSR5_comparator_value_3 = 1; // value that
uint8_t ADSR_Attack_3 = 0; // Attack value (0-15)
uint8_t ADSR_Decay_3 = 0; // Decay value (0-15)
uint8_t ADSR_Sustain_3 = 0; // Sustain value (0-15)
uint8_t ADSR_Release_3 = 0; // Release value (0-15)
uint8_t ADSR_stage_3;  // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
bool hold_zero_3 = false;

// gate bit

uint8_t Gate_bit_1 = 1; //
uint8_t Gate_bit_2 = 1;
uint8_t Gate_bit_3 = 1;
/*
   Gate_bit_X variable:
   0 - change from 1 to 0 * start Release stage
   1 - change from 0 to 1 * start Attack stage )

*/

uint8_t Gate_previous_1 = 0; // need to know what was previous gate bit
uint8_t Gate_previous_2 = 0; // need to know what was previous gate bit
uint8_t Gate_previous_3 = 0; // need to know what was previous gate bit


uint8_t waveform_switch_1 = 0; // 0-15, depending of waveform
uint8_t waveform_switch_2 = 0; // 0-15, depending of waveform
uint8_t waveform_switch_3 = 0; // 0-15, depending of waveform
//

uint16_t WaveformDA_1 = 0; // 12bit Waveform DA
uint16_t WaveformDA_noise_1 = 0;
uint16_t WaveformDA_triangle_1 = 0;
uint16_t WaveformDA_sawtooth_1 = 0;
uint16_t WaveformDA_pulse_1 = 0;
uint32_t pseudorandom_1 =  0x7ffff8; //  for rnd
uint32_t bit22_1 = 0;//
uint32_t bit17_1 = 0;
uint8_t bit_0_1 = 0;

uint16_t WaveformDA_2 = 0; // 12bit Waveform DA
uint16_t WaveformDA_noise_2 = 0;
uint16_t WaveformDA_triangle_2 = 0;
uint16_t WaveformDA_sawtooth_2 = 0;
uint16_t WaveformDA_pulse_2 = 0;
uint32_t pseudorandom_2 =  0x7ffff8; //  for rnd
uint32_t bit22_2 = 0;//
uint32_t bit17_2 = 0;
uint8_t bit_0_2 = 0;

uint16_t WaveformDA_3 = 0; // 12bit Waveform DA
uint16_t WaveformDA_noise_3 = 0;
uint16_t WaveformDA_triangle_3 = 0;
uint16_t WaveformDA_sawtooth_3 = 0;
uint16_t WaveformDA_pulse_3 = 0;
uint32_t pseudorandom_3 =  0x7ffff8; //  for rnd
uint32_t bit22_3 = 0;//
uint32_t bit17_3 = 0;
uint8_t bit_0_3 = 0;

// initial setup for SID registers - can be used for testing or for sketch with only interrupt (with just SID emulator, and direct change registers)
uint16_t sawtooth_bit_voice_1 = 0;
uint16_t triangle_bit_voice_1 = 0;
uint16_t pulse_bit_voice_1 = 0;
uint16_t noise_bit_voice_1 = 0;
uint16_t PW_HiLo_voice_1 = 2048;  // 2048 = 50% initial test

uint16_t sawtooth_bit_voice_2 = 0;
uint16_t triangle_bit_voice_2 = 0;
uint16_t pulse_bit_voice_2 = 0;
uint16_t noise_bit_voice_2 = 0;
uint16_t PW_HiLo_voice_2 = 2048;  // 2048 = 50% initial test

uint16_t sawtooth_bit_voice_3 = 0;
uint16_t triangle_bit_voice_3 = 0;
uint16_t pulse_bit_voice_3 = 0;
uint16_t noise_bit_voice_3 = 0;
uint16_t PW_HiLo_voice_3 = 2048;  // 2048 = 50% initial test


// ADSR volume (8bit) to LFSR5_comparator table
//

const uint8_t ADSR_Volume2LFSR5 [] = { // clock divider that drive ADSR_Volume counter, and is readed from ADSR_Volume value

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1e, 0x1e,
  0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x10, 0x10,
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
  0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01

};






#endif

/*
  //  ---------------------------------------------------------------------------
  //  This file is part of reSID, a MOS6581 SID emulator engine.
  //  Copyright (C) 2004  Dag Lem <resid@nimrod.no>
  //
  //  This program is free software; you can redistribute it and/or modify
  //  it under the terms of the GNU General Public License as published by
  //  the Free Software Foundation; either version 2 of the License, or
  //  (at your option) any later version.
  //
  //  This program is distributed in the hope that it will be useful,
  //  but WITHOUT ANY WARRANTY; without even the implied warranty of
  //  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  //  GNU General Public License for more details.
  //
  //  You should have received a copy of the GNU General Public License
  //  along with this program; if not, write to the Free Software
  //  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  //  ---------------------------------------------------------------------------


  reg8 WaveformGenerator::wave8580_PS_[] =
*/

// mask for combining waveforms
// from reSID, i couldn't find a pattern to calculate
// 8bit resolution (Upper 8 bits of Waveform generator, need to be <<4 shifted)
#ifndef _andmask
#define _andmask
const uint8_t AND_mask [] {
  /* 0x000: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x008: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x010: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x018: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x020: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x028: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x030: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x038: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x040: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x048: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x050: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x058: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x060: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x068: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x070: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x078: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x080: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x088: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x090: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x098: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x0c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x0f8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
  /* 0x100: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x108: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x110: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x118: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x120: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x128: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x130: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x138: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x140: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x148: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x150: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x158: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x160: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x168: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x170: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x178: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  /* 0x180: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x188: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x190: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x198: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x1c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x1e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x1f8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x1f,
  /* 0x200: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x208: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x210: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x218: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x220: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x228: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x230: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x238: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x240: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x248: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x250: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x258: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x260: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x268: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x270: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x278: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x280: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x288: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x290: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x298: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x2c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x2e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x2f8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f,
  /* 0x300: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x308: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x310: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x318: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x320: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x328: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x330: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x338: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x340: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x348: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x350: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x358: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x360: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x368: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x370: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x378: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17,
  /* 0x380: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x388: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x390: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x398: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b,
  /* 0x3c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3d,
  /* 0x3e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x3e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e,
  /* 0x3f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  /* 0x3f8: */  0x00, 0x0c, 0x1c, 0x3f, 0x1e, 0x3f, 0x3f, 0x3f,
  /* 0x400: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x408: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x410: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x418: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x420: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x428: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x430: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x438: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x440: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x448: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x450: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x458: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x460: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x468: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x470: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x478: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x480: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x488: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x490: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x498: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x4c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x4f8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
  /* 0x500: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x508: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x510: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x518: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x520: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x528: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x530: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x538: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x540: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x548: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x550: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x558: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x560: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x568: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x570: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x578: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  /* 0x580: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x588: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x590: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x598: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,
  /* 0x5c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,
  /* 0x5e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x5e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e,
  /* 0x5f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f,
  /* 0x5f8: */  0x00, 0x00, 0x00, 0x5f, 0x0c, 0x5f, 0x5f, 0x5f,
  /* 0x600: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x608: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x610: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x618: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x620: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x628: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x630: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x638: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x640: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x648: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x650: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x658: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x660: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x668: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x670: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x678: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47,
  /* 0x680: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x688: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x690: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x698: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43,
  /* 0x6c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65,
  /* 0x6e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x6e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e,
  /* 0x6f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f,
  /* 0x6f8: */  0x00, 0x40, 0x40, 0x6f, 0x40, 0x6f, 0x6f, 0x6f,
  /* 0x700: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x708: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x710: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x718: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x720: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x728: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x730: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x738: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63,
  /* 0x740: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x748: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x750: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x758: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x61,
  /* 0x760: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
  /* 0x768: */  0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x40, 0x70,
  /* 0x770: */  0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70,
  /* 0x778: */  0x40, 0x60, 0x60, 0x77, 0x60, 0x77, 0x77, 0x77,
  /* 0x780: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x788: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
  /* 0x790: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x60,
  /* 0x798: */  0x00, 0x40, 0x40, 0x60, 0x40, 0x60, 0x60, 0x79,
  /* 0x7a0: */  0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60,
  /* 0x7a8: */  0x40, 0x40, 0x40, 0x60, 0x60, 0x60, 0x60, 0x78,
  /* 0x7b0: */  0x40, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78,
  /* 0x7b8: */  0x60, 0x70, 0x70, 0x78, 0x70, 0x79, 0x7b, 0x7b,
  /* 0x7c0: */  0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x70,
  /* 0x7c8: */  0x60, 0x60, 0x60, 0x70, 0x60, 0x70, 0x70, 0x7c,
  /* 0x7d0: */  0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x7c,
  /* 0x7d8: */  0x70, 0x78, 0x78, 0x7c, 0x78, 0x7c, 0x7c, 0x7d,
  /* 0x7e0: */  0x70, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x7c,
  /* 0x7e8: */  0x78, 0x7c, 0x7c, 0x7e, 0x7c, 0x7e, 0x7e, 0x7e,
  /* 0x7f0: */  0x7c, 0x7c, 0x7c, 0x7e, 0x7e, 0x7f, 0x7f, 0x7f,
  /* 0x7f8: */  0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xff,
  /* 0x800: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x808: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x810: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x818: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x820: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x828: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x830: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x838: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x840: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x848: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x850: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x858: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x860: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x868: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x870: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x878: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  /* 0x880: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x888: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x890: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x898: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x8c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8e8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8f0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x8f8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f,
  /* 0x900: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x908: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x910: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x918: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x920: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x928: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x930: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x938: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0x940: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x948: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x950: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x958: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x960: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x968: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x970: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x978: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87,
  /* 0x980: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x988: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x990: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x998: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9a0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9a8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9b0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9b8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83,
  /* 0x9c0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9c8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9d0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0x9d8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x8d,
  /* 0x9e0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0x9e8: */  0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x8e,
  /* 0x9f0: */  0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x8f,
  /* 0x9f8: */  0x80, 0x80, 0x80, 0x9f, 0x80, 0x9f, 0x9f, 0x9f,
  /* 0xa00: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa08: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa10: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa18: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa20: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa28: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa30: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa38: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* 0xa40: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa48: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa50: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa58: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa60: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa68: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xa70: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xa78: */  0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x87,
  /* 0xa80: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa88: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa90: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xa98: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xaa0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xaa8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xab0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xab8: */  0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x83,
  /* 0xac0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xac8: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xad0: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
  /* 0xad8: */  0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81,
  /* 0xae0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xae8: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x84,
  /* 0xaf0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x87,
  /* 0xaf8: */  0x80, 0x80, 0x80, 0x87, 0x80, 0x8f, 0xaf, 0xaf,
  /* 0xb00: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* 0xb08: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xb10: */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  /* 0xb18: */  0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb20: */  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80,
  /* 0xb28: */  0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb30: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb38: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x83,
  /* 0xb40: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb48: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb50: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb58: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81,
  /* 0xb60: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb68: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xa0,
  /* 0xb70: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xa0,
  /* 0xb78: */  0x80, 0x80, 0x80, 0xa0, 0x80, 0xa3, 0xb7, 0xb7,
  /* 0xb80: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb88: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb90: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xb98: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xb1,
  /* 0xba0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xba8: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xb0,
  /* 0xbb0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xb0,
  /* 0xbb8: */  0x80, 0xa0, 0xa0, 0xb0, 0xa0, 0xb8, 0xb9, 0xbb,
  /* 0xbc0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xa0,
  /* 0xbc8: */  0x80, 0x80, 0x80, 0xa0, 0x80, 0xa0, 0xa0, 0xb8,
  /* 0xbd0: */  0x80, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xb8,
  /* 0xbd8: */  0xa0, 0xb0, 0xb0, 0xb8, 0xb0, 0xbc, 0xbc, 0xbd,
  /* 0xbe0: */  0xa0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb8, 0xb8, 0xbc,
  /* 0xbe8: */  0xb0, 0xb8, 0xb8, 0xbc, 0xb8, 0xbc, 0xbe, 0xbe,
  /* 0xbf0: */  0xb8, 0xbc, 0xbc, 0xbe, 0xbc, 0xbe, 0xbe, 0xbf,
  /* 0xbf8: */  0xbe, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
  /* 0xc00: */  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
  /* 0xc08: */  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
  /* 0xc10: */  0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc18: */  0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc20: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc28: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc30: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc38: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81,
  /* 0xc40: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc48: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc50: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc58: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc60: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc68: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc70: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc78: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc7,
  /* 0xc80: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc88: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc90: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xc98: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xca0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xca8: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xcb0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xcb8: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xc3,
  /* 0xcc0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xcc8: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xcd0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xcd8: */  0x80, 0x80, 0x80, 0xc0, 0x80, 0xc0, 0xc0, 0xc1,
  /* 0xce0: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xce8: */  0x80, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xcf0: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc7,
  /* 0xcf8: */  0xc0, 0xc0, 0xc0, 0xc7, 0xc0, 0xcf, 0xcf, 0xcf,
  /* 0xd00: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xd08: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xd10: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xd18: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xd20: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  /* 0xd28: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xd30: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xc0,
  /* 0xd38: */  0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc3,
  /* 0xd40: */  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0,
  /* 0xd48: */  0x80, 0x80, 0x80, 0xc0, 0x80, 0xc0, 0xc0, 0xc0,
  /* 0xd50: */  0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd58: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1,
  /* 0xd60: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd68: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd70: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd78: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc7, 0xd7,
  /* 0xd80: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd88: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd90: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xd98: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xda0: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xda8: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd0,
  /* 0xdb0: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd0,
  /* 0xdb8: */  0xc0, 0xc0, 0xc0, 0xd0, 0xc0, 0xd0, 0xd8, 0xdb,
  /* 0xdc0: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xdc8: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd8,
  /* 0xdd0: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd8,
  /* 0xdd8: */  0xc0, 0xc0, 0xc0, 0xd8, 0xd0, 0xd8, 0xd8, 0xdd,
  /* 0xde0: */  0xc0, 0xc0, 0xc0, 0xd0, 0xc0, 0xd0, 0xd0, 0xdc,
  /* 0xde8: */  0xd0, 0xd8, 0xd8, 0xdc, 0xd8, 0xdc, 0xdc, 0xde,
  /* 0xdf0: */  0xd8, 0xdc, 0xdc, 0xde, 0xdc, 0xde, 0xde, 0xdf,
  /* 0xdf8: */  0xde, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
  /* 0xe00: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe08: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe10: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe18: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe20: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe28: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe30: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe38: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe3,
  /* 0xe40: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe48: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  /* 0xe50: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0,
  /* 0xe58: */  0xc0, 0xc0, 0xc0, 0xe0, 0xc0, 0xe0, 0xe0, 0xe1,
  /* 0xe60: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0,
  /* 0xe68: */  0xc0, 0xc0, 0xc0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xe70: */  0xc0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xe78: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe1, 0xe3, 0xe7,
  /* 0xe80: */  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0,
  /* 0xe88: */  0xc0, 0xc0, 0xc0, 0xe0, 0xc0, 0xe0, 0xe0, 0xe0,
  /* 0xe90: */  0xc0, 0xc0, 0xc0, 0xe0, 0xc0, 0xe0, 0xe0, 0xe0,
  /* 0xe98: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xea0: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xea8: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xeb0: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xeb8: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xeb,
  /* 0xec0: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xec8: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xed0: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xed8: */  0xe0, 0xe0, 0xe0, 0xe8, 0xe0, 0xe8, 0xe8, 0xed,
  /* 0xee0: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xec,
  /* 0xee8: */  0xe0, 0xe0, 0xe0, 0xec, 0xe8, 0xec, 0xec, 0xee,
  /* 0xef0: */  0xe8, 0xe8, 0xe8, 0xec, 0xec, 0xee, 0xee, 0xef,
  /* 0xef8: */  0xec, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef,
  /* 0xf00: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xf08: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xf10: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
  /* 0xf18: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xf0,
  /* 0xf20: */  0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xf0,
  /* 0xf28: */  0xe0, 0xe0, 0xe0, 0xf0, 0xe0, 0xf0, 0xf0, 0xf0,
  /* 0xf30: */  0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
  /* 0xf38: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf3,
  /* 0xf40: */  0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
  /* 0xf48: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
  /* 0xf50: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
  /* 0xf58: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf5,
  /* 0xf60: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
  /* 0xf68: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf4, 0xf4, 0xf6,
  /* 0xf70: */  0xf0, 0xf0, 0xf0, 0xf4, 0xf0, 0xf4, 0xf6, 0xf7,
  /* 0xf78: */  0xf4, 0xf6, 0xf6, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
  /* 0xf80: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8,
  /* 0xf88: */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8,
  /* 0xf90: */  0xf0, 0xf0, 0xf0, 0xf8, 0xf0, 0xf8, 0xf8, 0xf8,
  /* 0xf98: */  0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf9,
  /* 0xfa0: */  0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
  /* 0xfa8: */  0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xfa,
  /* 0xfb0: */  0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xfb,
  /* 0xfb8: */  0xf8, 0xfa, 0xfa, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb,
  /* 0xfc0: */  0xf8, 0xf8, 0xf8, 0xfc, 0xf8, 0xfc, 0xfc, 0xfc,
  /* 0xfc8: */  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc,
  /* 0xfd0: */  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfd,
  /* 0xfd8: */  0xfc, 0xfc, 0xfc, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd,
  /* 0xfe0: */  0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
  /* 0xfe8: */  0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
  /* 0xff0: */  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  /* 0xff8: */  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// #include "xx_6502.h"
//
//////////////////////////////////////////////////////////////////////////////////////////////

// Blue6502
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _6502
#define _6502



uint8_t LOW_RAM = false;



uint16_t access_adress = 0x00;
uint16_t return_value = 0x00;


/* Contents of file MyROM.bin */
const int MyROM_size = 256;
const uint8_t MyROM[256] = {
  //  code and opcodes generated in Commodore 128's emulator, in Monitor (old habits die hard)

  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xA9, 0x00,         //  LDA#00
  0x20, 0x00, 0x08,   //  JSR $1000
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0x20, 0x03, 0x10,   //  JSR $1003 ($030c)
  0x18,               // CLC
  0x90, 0xF8,         // BCC 030C
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP
  0xEA,               //  NOP


  0x4C, 0x0E, 0xFF,   //  JMP $ff0e

  0xEA, 0xA2, 0x00, 0xA0, 0x00, 0xC8, 0xC0, 0xFF, 0xD0,
  0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00, 0xA0, 0x00, 0xC8, 0xC0, 0xFF,
  0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00, 0xA0, 0x00, 0xC8, 0xC0,
  0xFF, 0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00, 0xA0, 0x00, 0xC8,
  0xC0, 0xFF, 0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00, 0xA0, 0x00,
  0xC8, 0xC0, 0xFF, 0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00, 0xA0,
  0x00, 0xC8, 0xC0, 0xFF, 0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0xA2, 0x00,
  0xA0, 0x00, 0xC8, 0xC0, 0xFF, 0xD0, 0xFB, 0xE8, 0xE0, 0xFF, 0xD0, 0xF4, 0xEA, 0xEA, 0xEA, 0x4C,
  0x0E, 0xFF, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA,
  0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA,
  0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA,
  0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA,
  0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, // Filler
  0xEA, 0xEA, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x03,   // NMI Vector
  0x00, 0x03,   // Reset Vector (This will jump to program at 0x0300 )
  0x00, 0x03    // IRQ/BRK Vector
};

//6502 defines
#define UNDOCUMENTED //when this is defined, undocumented opcodes are handled.
//otherwise, they're simply treated as NOPs.

//flag modifier macros
#define setcarry() cpustatus |= FLAG_CARRY
#define clearcarry() cpustatus &= (~FLAG_CARRY)
#define setzero() cpustatus |= FLAG_ZERO
#define clearzero() cpustatus &= (~FLAG_ZERO)
#define setinterrupt() cpustatus |= FLAG_INTERRUPT
#define clearinterrupt() cpustatus &= (~FLAG_INTERRUPT)
#define setdecimal() cpustatus |= FLAG_DECIMAL
#define cleardecimal() cpustatus &= (~FLAG_DECIMAL)
#define setoverflow() cpustatus |= FLAG_OVERFLOW
#define clearoverflow() cpustatus &= (~FLAG_OVERFLOW)
#define setsign() cpustatus |= FLAG_SIGN
#define clearsign() cpustatus &= (~FLAG_SIGN)


#define  saveaccum( n)  ACCUMULATOR = ((uint8_t)(n) & 0x00FF);

//flag calculation macros
#define zerocalc( n)  if ((n) & 0x00FF) clearzero();  else setzero();


#define  signcalc( n)  {    if ((n) & 0x0080) setsign();    else clearsign();  }

#define carrycalc( n)  {    if ((n) & 0xFF00) setcarry();    else clearcarry();  }

#define  overflowcalc( n,  m,  o)   { if (((n) ^ (m)) & ((n) ^ (o)) & 0x0080) setoverflow();    else clearoverflow();  }

uint8_t FLAG_CARRY    = 0x01;
uint8_t  FLAG_ZERO    =  0x02;
uint8_t  FLAG_INTERRUPT = 0x04;
uint8_t  FLAG_DECIMAL  = 0x08;
uint8_t  FLAG_BREAK     = 0x10;
uint8_t  FLAG_CONSTANT  = 0x20;
uint8_t  FLAG_OVERFLOW = 0x40;
uint8_t  FLAG_SIGN     = 0x80;

uint16_t  BASE_STACK    = 0x100;




uint8_t  ACCUMULATOR = 0;
uint8_t X_REGISTER = 0;
uint8_t Y_REGISTER = 0;
uint8_t  STACK_POINTER = 0xFD;
uint8_t cpustatus;
//helper variables
uint32_t instructions = 0; //keep track of total instructions executed
int32_t clockticks6502 = 0, clockgoal6502 = 0;
uint16_t oldPROGRAM_COUNTER, ea, reladdr, value6502, result6502;
uint8_t opcode, oldcpustatus, useaccum;
uint16_t PROGRAM_COUNTER;
// end 0f Blue6502 defines and variable declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// globaling locals (make local variables global, to minimize push to stack time
uint16_t temp16;
uint16_t startpage;
uint16_t eahelp, eahelp2;

const uint8_t ticktable[256] = {
  /*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
  /* 0 */      7,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    4,    4,    6,    6,  /* 0 */
  /* 1 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 1 */
  /* 2 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    4,    4,    6,    6,  /* 2 */
  /* 3 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 3 */
  /* 4 */      6,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    3,    4,    6,    6,  /* 4 */
  /* 5 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 5 */
  /* 6 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    5,    4,    6,    6,  /* 6 */
  /* 7 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 7 */
  /* 8 */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* 8 */
  /* 9 */      2,    6,    2,    6,    4,    4,    4,    4,    2,    5,    2,    5,    5,    5,    5,    5,  /* 9 */
  /* A */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* A */
  /* B */      2,    5,    2,    5,    4,    4,    4,    4,    2,    4,    2,    4,    4,    4,    4,    4,  /* B */
  /* C */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* C */
  /* D */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* D */
  /* E */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* E */
  /* F */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7   /* F */
};





#endif
#ifdef __cplusplus
} // extern "C"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 02_ram.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////


#if defined(SPI_RAM)
// TODO
#elif defined(PARALLEL_SRAM)
// TODO
#else
// internal RAM

void AllocateRAM() {
  if ((FreeBytes()) > 0xffff ) {
    RAM_SIZE = 0xffff;
  }
  else {
    RAM_SIZE = FreeBytes() - FREE_RAM;
  }
  RAM = (uint8_t*) calloc(RAM_SIZE, sizeof(uint8_t)); // allocate memory
}

inline void POKE (uint16_t addr , uint8_t bytE ) {

  RAM[addr] = bytE;
}

inline uint8_t PEEK (uint16_t addr ) {
  return RAM[addr];
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 03_6502.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////

// Fake6502 emulator
// http://rubbermallet.org/fake6502.c

// 6502 emulation
// far from perfect, it runs very slow, once interrupts are enabled, compared to 1Mhz CPU, but it is enough to do the job (emulation is needed only in few raster lines only anyway, around 500 instructions)
// I only edited part with read and write so it can act uppon addresses


inline  uint8_t read6502(uint16_t address) {

  return_value = 0x60; // if it's not in any of knowed area, assume it's 0x60 (RTS) (Dan Dare 3 fix)

  if (address == 0x030c) { // player's  sid-play subroutine's adress
    JSR1003 = 1; // if it's loading <20-03-10> (example: JSR $1003) value from sid_play routine, it is time for fake VIC-II irq signal
  }
  if ((address >= 0xD400) && (address < 0xD420)) {

    STAD4XX = 1; // sid read
    return_value = SID[address - 0xD400]; // TODO: make it unreadable //   SID
  }

  if ( (address >=  SID_load_start)  & (address <  SID_load_end ) ) { // sid player area

    if ( LOW_RAM == true ) { // hack to relocate addresses from SID_start to 0x400

      if ( (0x400 + address - SID_load_start) < (RAM_SIZE )) { // read from RAM if it's in available RAM,
        return_value = PEEK (0x400 + address - SID_load_start) ;
      }
      else { // otherwise read from SID[] array

        return_value =   SID_data[ 0x7e + address - SID_load_start]   ;

      }
    }

  }


  if ( LOW_RAM == false ) {
    if (address <= (RAM_SIZE )) return_value = PEEK (address) ; // for  memory space that is covered by RAM

  }

  if ( address < (0x400) ) return_value = PEEK (address) ; // zero page, stack, player, screen RAM

  //CIA timer
  if ( address == 0xdc04 )  {
    return_value = CIA_DC04;
  }
  if ( address == 0xdc05 )  {
    return_value = CIA_DC05;
  }





  return return_value;

}


inline void write6502(uint16_t address, uint8_t value6502) {



  if ((address >= 0xD400) & (address < 0xD420)) { // writing to sid - assagn values for interrupt
    //STAD4XX = 1; // SID write signal for IRQ

    // SID MAGIC
    access_adress = (address - 0xD400);
    SID[ (access_adress)] = value6502; //  SID


    // disable if IRQ is transfering SID[] variable
    switch (access_adress) {

      case 0:
        OSC_1_HiLo = ((SID[0] & 0xff) + ( (SID[1] & 0xff) << 8) ); // *0.985
        break;
      case 1:
        OSC_1_HiLo = ((SID[0] & 0xff) + ( (SID[1] & 0xff) << 8)); // *0.985
        break;
      case 2:
        PW_HiLo_voice_1 = SID[2] + (((SID[3] & 0x0f) << 8 ));
        break;
      case 3:
        PW_HiLo_voice_1 = SID[2] + (( (SID[3] & 0x0f) << 8 ));
        break;
      case 4:
        STAD4XX = 1;
        noise_bit_voice_1 = ( (SID[4] >> 7 ) & 1) ;
        pulse_bit_voice_1 = ( (SID[4] >> 6 ) & 1) ;
        sawtooth_bit_voice_1 = ( (SID[4] >> 5 ) & 1) ;
        triangle_bit_voice_1 = ( (SID[4] >> 4 ) & 1) ;
        test_bit_voice_1   = ( (SID[4] >> 3 ) & 1) ; //
        ring_bit_voice_1   = ( (SID[4] >> 2 ) & 1) ;
        SYNC_bit_voice_1 = ( (SID[4] >> 1 ) & 1) ; //
        Gate_bit_1 = SID[4] & 1;   //

        //waveform_switch_1 = (noise_bit_voice_1 << 3) | (pulse_bit_voice_1 << 2) | (sawtooth_bit_voice_1 << 1) | (triangle_bit_voice_1); // for barebone version
        //waveform_switch_1 =  ( SID[4] >> 4); // it's in IRQ

        break;
      case 5:
        ADSR_Attack_1 = ( (SID[5] >> 4 ) & 0x0f) ;
        ADSR_Decay_1 = ( (SID[5]  ) & 0x0f) ;
        break;
      case 6:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_1 = ( (SID[6] >> 4 ) & 0x0f) ;
        ADSR_Release_1 = ( (SID[6]  ) & 0x0f);
        break;
      case 7:
        OSC_2_HiLo = (SID[7] + ( SID[8] << 8)) ; // PAL speed recalc // *0.985
        break;
      case 8:
        OSC_2_HiLo = (SID[7] + ( SID[8] << 8)) ; // *0.985
        break;
      case 9:
        PW_HiLo_voice_2 = SID[9] + ( (SID[10] & 0x0f) << 8);
        break;
      case 10:
        PW_HiLo_voice_2 = SID[9] + ( (SID[10] & 0x0f) << 8);
        break;
      case 11:
        STAD4XX = 1;
        noise_bit_voice_2 = ( (SID[11] >> 7 ) & 1) ;
        pulse_bit_voice_2 = ( (SID[11] >> 6 ) & 1) ;
        sawtooth_bit_voice_2 = ( (SID[11] >> 5 ) & 1) ;
        triangle_bit_voice_2 = ( (SID[11] >> 4 ) & 1) ;
        test_bit_voice_2   = ( (SID[11] >> 3 ) & 1) ; //
        ring_bit_voice_2 = ( (SID[11] >> 2 ) & 1) ; //
        SYNC_bit_voice_2 = ( (SID[11] >> 1 ) & 1) ; //
        Gate_bit_2 = SID[11] & 1;   //

        //waveform_switch_2 = (noise_bit_voice_2 << 3) | (pulse_bit_voice_2 << 2) | (sawtooth_bit_voice_2 << 1) | (triangle_bit_voice_2);
        //waveform_switch_2 = 0x0f & ( SID[11] >> 4);

        break;
      case 12:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Attack_2 = ( (SID[12] >> 4 ) & 0x0f) ;
        ADSR_Decay_2 = ( (SID[12]  ) & 0x0f) ;
        break;
      case 13:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_2 = ( (SID[13] >> 4 ) & 0x0f) ;
        ADSR_Release_2 = ( (SID[13]  ) & 0x0f);
        break;
      case 14:
        OSC_3_HiLo = (SID[14] + ( SID[15] << 8)) ; // *0.985
        break;
      case 15:
        OSC_3_HiLo = (SID[14] + ( SID[15] << 8)) ; // *0.985
        break;
      case 16:
        PW_HiLo_voice_3 = SID[16] + ( (SID[17] & 0x0f) << 8);
        break;
      case 17:
        PW_HiLo_voice_3 = SID[16] + ( (SID[17] & 0x0f) << 8);
        break;
      case 18:
        STAD4XX = 1;
        noise_bit_voice_3 = ( (SID[18] >> 7 ) & 1) ;
        pulse_bit_voice_3 = ( (SID[18] >> 6 ) & 1) ;
        sawtooth_bit_voice_3 = ( (SID[18] >> 5 ) & 1) ;
        triangle_bit_voice_3 = ( (SID[18] >> 4 ) & 1) ;
        test_bit_voice_3  = ( (SID[18] >> 3 ) & 1) ; //
        ring_bit_voice_3 = ( (SID[18] >> 2 ) & 1) ; //
        SYNC_bit_voice_3 = ( (SID[18] >> 1 ) & 1) ; //
        Gate_bit_3 = SID[18] & 1;   //

        //waveform_switch_3 = (noise_bit_voice_3 << 3) | (pulse_bit_voice_3 << 2) | (sawtooth_bit_voice_3 << 1) | (triangle_bit_voice_3); // for barebone version
        //waveform_switch_3 = 0x0f & ( SID[18] >> 4);
        break;
      case 19:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Attack_3 = ( (SID[19] >> 4 ) & 0x0f) ;
        ADSR_Decay_3 = ( (SID[19]  ) & 0x0f) ;
        break;
      case 20:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_3 = ( (SID[20] >> 4 ) & 0x0f) ;
        ADSR_Release_3 = ( (SID[20]  ) & 0x0f);
        break;
      case 21:
        FILTER_HiLo = (SID[21] & 0x07) + ( SID[22] << 3); // 11bit // TODO

        //set w0
        // from reSID
        // Multiply with 1.048576 to facilitate division by 1 000 000 by right-
        // shifting 20 times (2 ^ 20 = 1048576).
        // w0 = static_cast<sound_sample>(2*pi*f0[fc]*1.048576);
        w0 = w0_constant_part * (FILTER_HiLo + 0x01); // 0x01 offset testing
        // w0_ceil_dt = w0 <= w0_max_dt ? w0 : w0_max_dt;
        if (w0 < w0_max_dt) {
          w0_ceil_dt = w0;
        }
        else {
          w0_ceil_dt = w0_max_dt;
        }

        break;
      case 22:
        FILTER_HiLo = (SID[21] & 0x07) + (SID[22] << 3); // 11bit // TODO

        //set w0
        // Multiply with 1.048576 to facilitate division by 1 000 000 by right-
        // shifting 20 times (2 ^ 20 = 1048576).
        // w0 = static_cast<sound_sample>(2*pi*f0[fc]*1.048576);
        w0 = w0_constant_part * (FILTER_HiLo + 0x1); //

        // w0_ceil_dt = w0 <= w0_max_dt ? w0 : w0_max_dt;
        if (w0 < w0_max_dt) {
          w0_ceil_dt = w0;
        }
        else {
          w0_ceil_dt = w0_max_dt;
        }

        break;
      case 23:

        FILTER_Resonance = ( (SID[23] >> 4 ) & 0x0f) ;; // 4bit // TODO
        FILTER_Enable_1 =  SID[23]  & 1 ; // on/off
        FILTER_Enable_2 = ( (SID[23] >> 1 ) & 1) ;; // on/off
        FILTER_Enable_3 = ( (SID[23] >> 2 ) & 1) ;; // on/off
        FILTER_Enable_EXT = ( (SID[23] >> 3 ) & 1) ;; // on/off

        FILTER_Enable_switch =  SID[23]  & 0x07 ; // for filter switch in irq (no external input filter)

        // The coefficient 1024 is dispensed of later by right-shifting 10 times
        // (2 ^ 10 = 1024).
        // _1024_div_Q = static_cast<sound_sample>(1024.0/(0.707 + 1.0*res/0x0f));
        Q_1024_div = (1024.0 / (0.707 + 1.0 * (FILTER_Resonance) / 15.0));

        break;
      case 24:
        //STAD4XX = 1; // SID write signal for IRQ
        OFF3 =  ( (SID[24] >> 7 ) & 1) ;; // on/off; //
        FILTER_HP =  ( (SID[24] >> 6 ) & 1) ;; // on/off; //;
        FILTER_BP =  ( (SID[24] >> 5 ) & 1) ;; // on/off; //;
        FILTER_LP =  ( (SID[24] >> 4 ) & 1) ;; // on/off; //;
        MASTER_VOLUME =   (SID[24]  & 15) ;; // on/off; //;
        // change volume immidiattelly
        //main_volume = MASTER_VOLUME * ( main_volume_32bit) / 15;
        //TIMER1_BASE->CCR1 =  main_volume;
        // disable if there is there is no delay hack
        //STAD4XX = 1;

        break;
      case 25:

        break;
      case 26:

        break;
      case 27:

        break;
      case 28:

        break;
      case 29:

        break;
      case 30:

        break;
      case 31:
        break;
    }

    // STAD4XX = 1; // SID write signal for IRQ
    //PB13_HIGH;
  }


  if ( LOW_RAM == true ) {
    if ( (address >=  SID_load_start)   & (address <  SID_load_end ) ) {
      if ( 0x400 + address - SID_load_start < (RAM_SIZE )) { // write to memory only if it fits into RAM, rest is ignored.
        POKE (0x400 + address - SID_load_start, value6502); // sid data memory space
      }
    }

    if (address < 0x400) POKE (address, value6502); // zero page, stack, player and free ram
  } // LOW_RAM



  if ( LOW_RAM == false ) {
    if (address <= (RAM_SIZE )) {
      POKE (address, value6502); // for  memory space that is covered by RAM
    }
  } // LOW_RAM


  //CIA timer
  if ( address == 0xdc04 )  {
    CIA_DC04 = value6502;
  }
  if ( address == 0xdc05 )  {
    CIA_DC05 = value6502;
    if (CIA_DC05 > 0) { // set song speed only when Hi value of CIA timer is greater then 0, and only on write to $DC05)
      set_tune_speed ();
    }
  }

}


// Blue6502's voids // i didn't change much of Fake6502's code, it's working good

inline void reset6502() {

  PROGRAM_COUNTER = 0x0300;  // player is copied to RAM at 0x300
  //PROGRAM_COUNTER = (uint16_t)read6502(0xFFFC) | ((uint16_t)read6502(0xFFFD) << 8); // reset vector not used
  ACCUMULATOR = 0;
  X_REGISTER = 0;
  Y_REGISTER = 0;
  STACK_POINTER = 0xFD;
  cpustatus |= FLAG_CONSTANT;

  CIA_DC04 = 0;
  CIA_DC05 = 0;


}




//a few general functions used by various other functions
inline void push16(uint16_t pushval) {
  write6502(BASE_STACK + STACK_POINTER, (pushval >> 8) & 0xFF);
  write6502(BASE_STACK + ((STACK_POINTER - 1) & 0xFF), pushval & 0xFF);
  STACK_POINTER -= 2;
}

inline void push8(uint8_t pushval) {
  write6502(BASE_STACK + STACK_POINTER--, pushval);
}

inline uint16_t pull16() {

  temp16 = read6502(BASE_STACK + ((STACK_POINTER + 1) & 0xFF)) | (read6502(BASE_STACK + ((STACK_POINTER + 2) & 0xFF)) << 8);
  STACK_POINTER += 2;
  return (temp16);
}

inline uint8_t pull8() {
  return (read6502(BASE_STACK + ++STACK_POINTER));
}



//addressing mode functions, calculates effective addresses
inline void imp() { //implied
}

inline void acc() { //accumulator
  useaccum = 1;
}

inline void imm() { //immediate
  ea = PROGRAM_COUNTER++;
}

inline void zp() { //zero-page
  ea = read6502(PROGRAM_COUNTER++);
}

inline void zpx() { //zero-page,X
  ea = (read6502(PROGRAM_COUNTER++) + X_REGISTER) & 0xFF; //zero-page wraparound
}

inline void zpy() { //zero-page,Y
  ea = (read6502(PROGRAM_COUNTER++) + Y_REGISTER) & 0xFF; //zero-page wraparound
}

inline void rel() { //relative for branch ops (8-bit immediate value, sign-extended)
  reladdr = read6502(PROGRAM_COUNTER++);
  if (reladdr & 0x80) reladdr |= 0xFF00;
}

inline void abso() { //absolute
  ea = read6502(PROGRAM_COUNTER) | (read6502(PROGRAM_COUNTER + 1) << 8);
  PROGRAM_COUNTER += 2;
}

inline void absx() { //absolute,X

  ea = (read6502(PROGRAM_COUNTER) | (read6502(PROGRAM_COUNTER + 1) << 8));
  startpage = ea & 0xFF00;
  ea += X_REGISTER;

  PROGRAM_COUNTER += 2;
}

inline void absy() { //absolute,Y

  ea = (read6502(PROGRAM_COUNTER) | (read6502(PROGRAM_COUNTER + 1) << 8));
  startpage = ea & 0xFF00;
  ea += Y_REGISTER;

  PROGRAM_COUNTER += 2;
}

inline void ind() { //indirect

  eahelp = read6502(PROGRAM_COUNTER) | (read6502(PROGRAM_COUNTER + 1) << 8);
  eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //replicate 6502 page-boundary wraparound bug
  ea = read6502(eahelp) | (read6502(eahelp2) << 8);
  PROGRAM_COUNTER += 2;
}

inline void indx() { // (indirect,X)

  eahelp = ((read6502(PROGRAM_COUNTER++) + X_REGISTER) & 0xFF); //zero-page wraparound for table pointer
  ea = read6502(eahelp & 0x00FF) | (read6502((eahelp + 1) & 0x00FF) << 8);
}

inline void indy() { // (indirect),Y

  eahelp = read6502(PROGRAM_COUNTER++);
  eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //zero-page wraparound
  ea = read6502(eahelp) | (read6502(eahelp2) << 8);
  startpage = ea & 0xFF00;
  ea += Y_REGISTER;

}

uint16_t getvalue6502() {
  if (useaccum) return (ACCUMULATOR);
  else return (read6502(ea));
}

uint16_t getvalue16() {
  return (read6502(ea) | (read6502(ea + 1) << 8));
}

inline void putvalue(uint16_t saveval) {
  if (useaccum) ACCUMULATOR = (saveval & 0x00FF);
  else write6502(ea, (saveval & 0x00FF));
}


//instruction handler functions
inline void adc() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR + value6502 + (cpustatus & FLAG_CARRY);

  carrycalc(result6502);
  zerocalc(result6502);
  overflowcalc(result6502, ACCUMULATOR, value6502);
  signcalc(result6502);
  saveaccum(result6502);
}


inline void op_and() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR & value6502;

  zerocalc(result6502);
  signcalc(result6502);

  saveaccum(result6502);
}

inline void asl() {
  value6502 = getvalue6502();
  result6502 = value6502 << 1;

  carrycalc(result6502);
  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void bcc() {
  if ((cpustatus & FLAG_CARRY) == 0) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bcs() {
  if ((cpustatus & FLAG_CARRY) == FLAG_CARRY) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void beq() {
  if ((cpustatus & FLAG_ZERO) == FLAG_ZERO) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void op_bit() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR & value6502;

  zerocalc(result6502);
  cpustatus = (cpustatus & 0x3F) | (value6502 & 0xC0);
}

inline void bmi() {
  if ((cpustatus & FLAG_SIGN) == FLAG_SIGN) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bne() {
  if ((cpustatus & FLAG_ZERO) == 0) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bpl() {
  if ((cpustatus & FLAG_SIGN) == 0) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void brk() {
  PROGRAM_COUNTER++;
  push16(PROGRAM_COUNTER); //push next instruction address onto stack
  push8(cpustatus | FLAG_BREAK); //push CPU cpustatus to stack
  setinterrupt(); //set interrupt flag
  PROGRAM_COUNTER = read6502(0xFFFE) | (read6502(0xFFFF) << 8);
}

inline void bvc() {
  if ((cpustatus & FLAG_OVERFLOW) == 0) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bvs() {
  if ((cpustatus & FLAG_OVERFLOW) == FLAG_OVERFLOW) {
    oldPROGRAM_COUNTER = PROGRAM_COUNTER;
    PROGRAM_COUNTER += reladdr;
    if ((oldPROGRAM_COUNTER & 0xFF00) != (PROGRAM_COUNTER & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void clc() {
  clearcarry();
}

inline void cld() {
  cleardecimal();
}

inline void cli() {
  clearinterrupt();
}

inline void clv() {
  clearoverflow();
}

inline void cmp() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR - value6502;

  if (ACCUMULATOR >= (value6502 & 0x00FF)) setcarry();
  else clearcarry();
  if (ACCUMULATOR == (value6502 & 0x00FF)) setzero();
  else clearzero();
  signcalc(result6502);
}

inline void cpx() {
  value6502 = getvalue6502();
  result6502 = X_REGISTER - value6502;

  if (X_REGISTER >= (value6502 & 0x00FF)) setcarry();
  else clearcarry();
  if (X_REGISTER == (value6502 & 0x00FF)) setzero();
  else clearzero();
  signcalc(result6502);
}

inline void cpy() {
  value6502 = getvalue6502();
  result6502 = Y_REGISTER - value6502;

  if (Y_REGISTER >= (value6502 & 0x00FF)) setcarry();
  else clearcarry();
  if (Y_REGISTER == (value6502 & 0x00FF)) setzero();
  else clearzero();
  signcalc(result6502);
}

inline void dec() {
  value6502 = getvalue6502();
  result6502 = value6502 - 1;

  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void dex() {
  X_REGISTER--;

  zerocalc(X_REGISTER);
  signcalc(X_REGISTER);
}

inline void dey() {
  Y_REGISTER--;

  zerocalc(Y_REGISTER);
  signcalc(Y_REGISTER);
}

inline void eor() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR ^ value6502;

  zerocalc(result6502);
  signcalc(result6502);

  saveaccum(result6502);
}

inline void inc() {
  value6502 = getvalue6502();
  result6502 = value6502 + 1;

  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void inx() {
  X_REGISTER++;

  zerocalc(X_REGISTER);
  signcalc(X_REGISTER);
}

inline void iny() {
  Y_REGISTER++;

  zerocalc(Y_REGISTER);
  signcalc(Y_REGISTER);
}

inline void jmp() {
  PROGRAM_COUNTER = ea;
}

inline void jsr() {
  push16(PROGRAM_COUNTER - 1);
  PROGRAM_COUNTER = ea;
}

inline void lda() {
  value6502 = getvalue6502();
  ACCUMULATOR = (value6502 & 0x00FF);

  zerocalc(ACCUMULATOR);
  signcalc(ACCUMULATOR);
}

inline void ldx() {
  value6502 = getvalue6502();
  X_REGISTER = (value6502 & 0x00FF);

  zerocalc(X_REGISTER);
  signcalc(X_REGISTER);
}

inline void ldy() {
  value6502 = getvalue6502();
  Y_REGISTER = (value6502 & 0x00FF);

  zerocalc(Y_REGISTER);
  signcalc(Y_REGISTER);
}

inline void lsr() {
  value6502 = getvalue6502();
  result6502 = value6502 >> 1;

  if (value6502 & 1) setcarry();
  else clearcarry();
  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void nop() {
}

inline void ora() {
  value6502 = getvalue6502();
  result6502 = ACCUMULATOR | value6502;

  zerocalc(result6502);
  signcalc(result6502);

  saveaccum(result6502);
}

inline void pha() {
  push8(ACCUMULATOR);
}

inline void php() {
  push8(cpustatus | FLAG_BREAK);
}

inline void pla() {
  ACCUMULATOR = pull8();

  zerocalc(ACCUMULATOR);
  signcalc(ACCUMULATOR);
}

inline void plp() {
  cpustatus = pull8() | FLAG_CONSTANT;
}

inline void rol() {
  value6502 = getvalue6502();
  result6502 = (value6502 << 1) | (cpustatus & FLAG_CARRY);

  carrycalc(result6502);
  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void ror() {
  value6502 = getvalue6502();
  result6502 = (value6502 >> 1) | ((cpustatus & FLAG_CARRY) << 7);

  if (value6502 & 1) setcarry();
  else clearcarry();
  zerocalc(result6502);
  signcalc(result6502);

  putvalue(result6502);
}

inline void rti() {
  cpustatus = pull8();
  value6502 = pull16();
  PROGRAM_COUNTER = value6502;
}

inline void rts() {
  value6502 = pull16();
  PROGRAM_COUNTER = value6502 + 1;
}

inline void sbc() {
  value6502 = getvalue6502() ^ 0x00FF;
  result6502 = ACCUMULATOR + value6502 + (cpustatus & FLAG_CARRY);

  carrycalc(result6502);
  zerocalc(result6502);
  overflowcalc(result6502, ACCUMULATOR, value6502);
  signcalc(result6502);



  saveaccum(result6502);
}


inline void sec() {
  setcarry();
}

inline void sed() {
  setdecimal();
}

inline void sei() {
  setinterrupt();
}

inline void sta() {
  putvalue(ACCUMULATOR);
}

inline void stx() {
  putvalue(X_REGISTER);
}

inline void sty() {
  putvalue(Y_REGISTER);
}

inline void tax() {
  X_REGISTER = ACCUMULATOR;

  zerocalc(X_REGISTER);
  signcalc(X_REGISTER);
}

inline void tay() {
  Y_REGISTER = ACCUMULATOR;

  zerocalc(Y_REGISTER);
  signcalc(Y_REGISTER);
}

inline void tsx() {
  X_REGISTER = STACK_POINTER;

  zerocalc(X_REGISTER);
  signcalc(X_REGISTER);
}

inline void txa() {
  ACCUMULATOR = X_REGISTER;

  zerocalc(ACCUMULATOR);
  signcalc(ACCUMULATOR);
}

inline void txs() {
  STACK_POINTER = X_REGISTER;
}

inline void tya() {
  ACCUMULATOR = Y_REGISTER;

  zerocalc(ACCUMULATOR);
  signcalc(ACCUMULATOR);
}

//undocumented instructions
#ifdef UNDOCUMENTED
inline void lax() {
  lda();
  ldx();
}

inline void sax() {
  sta();
  stx();
  putvalue(ACCUMULATOR & X_REGISTER);
}

inline void dcp() {
  dec();
  cmp();
}

inline void isb() {
  inc();
  sbc();
}

inline void slo() {
  asl();
  ora();
}

inline void rla() {
  rol();
  op_and();
}

inline void sre() {
  lsr();
  eor();
}

inline void rra() {
  ror();
  adc();
}
#else
#define lax nop
#define sax nop
#define dcp nop
#define isb nop
#define slo nop
#define rla nop
#define sre nop
#define rra nop
#endif


inline void exec6502() {



  opcode = read6502(PROGRAM_COUNTER++);

  cpustatus |= FLAG_CONSTANT;

  useaccum = 0;

  switch (opcode) {
    case 0x0:
      imp();
      brk();
      break;
    case 0x1:
      indx();
      ora();
      break;
    case 0x5:
      zp();
      ora();
      break;
    case 0x6:
      zp();
      asl();
      break;
    case 0x8:
      imp();
      php();
      break;
    case 0x9:
      imm();
      ora();
      break;
    case 0xA:
      acc();
      asl();
      break;
    case 0xD:
      abso();
      ora();
      break;
    case 0xE:
      abso();
      asl();
      break;
    case 0x10:
      rel();
      bpl();
      break;
    case 0x11:
      indy();
      ora();
      break;
    case 0x15:
      zpx();
      ora();
      break;
    case 0x16:
      zpx();
      asl();
      break;
    case 0x18:
      imp();
      clc();
      break;
    case 0x19:
      absy();
      ora();
      break;
    case 0x1D:
      absx();
      ora();
      break;
    case 0x1E:
      absx();
      asl();
      break;
    case 0x20:
      abso();
      jsr();
      break;
    case 0x21:
      indx();
      op_and();
      break;
    case 0x24:
      zp();
      op_bit();
      break;
    case 0x25:
      zp();
      op_and();
      break;
    case 0x26:
      zp();
      rol();
      break;
    case 0x28:
      imp();
      plp();
      break;
    case 0x29:
      imm();
      op_and();
      break;
    case 0x2A:
      acc();
      rol();
      break;
    case 0x2C:
      abso();
      op_bit();
      break;
    case 0x2D:
      abso();
      op_and();
      break;
    case 0x2E:
      abso();
      rol();
      break;
    case 0x30:
      rel();
      bmi();
      break;
    case 0x31:
      indy();
      op_and();
      break;
    case 0x35:
      zpx();
      op_and();
      break;
    case 0x36:
      zpx();
      rol();
      break;
    case 0x38:
      imp();
      sec();
      break;
    case 0x39:
      absy();
      op_and();
      break;
    case 0x3D:
      absx();
      op_and();
      break;
    case 0x3E:
      absx();
      rol();
      break;
    case 0x40:
      imp();
      rti();
      break;
    case 0x41:
      indx();
      eor();
      break;
    case 0x45:
      zp();
      eor();
      break;
    case 0x46:
      zp();
      lsr();
      break;
    case 0x48:
      imp();
      pha();
      break;
    case 0x49:
      imm();
      eor();
      break;
    case 0x4A:
      acc();
      lsr();
      break;
    case 0x4C:
      abso();
      jmp();
      break;
    case 0x4D:
      abso();
      eor();
      break;
    case 0x4E:
      abso();
      lsr();
      break;
    case 0x50:
      rel();
      bvc();
      break;
    case 0x51:
      indy();
      eor();
      break;
    case 0x55:
      zpx();
      eor();
      break;
    case 0x56:
      zpx();
      lsr();
      break;
    case 0x58:
      imp();
      cli();
      break;
    case 0x59:
      absy();
      eor();
      break;
    case 0x5D:
      absx();
      eor();
      break;
    case 0x5E:
      absx();
      lsr();
      break;
    case 0x60:
      imp();
      rts();
      break;
    case 0x61:
      indx();
      adc();
      break;
    case 0x65:
      zp();
      adc();
      break;
    case 0x66:
      zp();
      ror();
      break;
    case 0x68:
      imp();
      pla();
      break;
    case 0x69:
      imm();
      adc();
      break;
    case 0x6A:
      acc();
      ror();
      break;
    case 0x6C:
      ind();
      jmp();
      break;
    case 0x6D:
      abso();
      adc();
      break;
    case 0x6E:
      abso();
      ror();
      break;
    case 0x70:
      rel();
      bvs();
      break;
    case 0x71:
      indy();
      adc();
      break;
    case 0x75:
      zpx();
      adc();
      break;
    case 0x76:
      zpx();
      ror();
      break;
    case 0x78:
      imp();
      sei();
      break;
    case 0x79:
      absy();
      adc();
      break;
    case 0x7D:
      absx();
      adc();
      break;
    case 0x7E:
      absx();
      ror();
      break;
    case 0x81:
      indx();
      sta();
      break;
    case 0x84:
      zp();
      sty();
      break;
    case 0x85:
      zp();
      sta();
      break;
    case 0x86:
      zp();
      stx();
      break;
    case 0x88:
      imp();
      dey();
      break;
    case 0x8A:
      imp();
      txa();
      break;
    case 0x8C:
      abso();
      sty();
      break;
    case 0x8D:
      abso();
      sta();
      break;
    case 0x8E:
      abso();
      stx();
      break;
    case 0x90:
      rel();
      bcc();
      break;
    case 0x91:
      indy();
      sta();
      break;
    case 0x94:
      zpx();
      sty();
      break;
    case 0x95:
      zpx();
      sta();
      break;
    case 0x96:
      zpy();
      stx();
      break;
    case 0x98:
      imp();
      tya();
      break;
    case 0x99:
      absy();
      sta();
      break;
    case 0x9A:
      imp();
      txs();
      break;
    case 0x9D:
      absx();
      sta();
      break;
    case 0xA0:
      imm();
      ldy();
      break;
    case 0xA1:
      indx();
      lda();
      break;
    case 0xA2:
      imm();
      ldx();
      break;
    case 0xA4:
      zp();
      ldy();
      break;
    case 0xA5:
      zp();
      lda();
      break;
    case 0xA6:
      zp();
      ldx();
      break;
    case 0xA8:
      imp();
      tay();
      break;
    case 0xA9:
      imm();
      lda();
      break;
    case 0xAA:
      imp();
      tax();
      break;
    case 0xAC:
      abso();
      ldy();
      break;
    case 0xAD:
      abso();
      lda();
      break;
    case 0xAE:
      abso();
      ldx();
      break;
    case 0xB0:
      rel();
      bcs();
      break;
    case 0xB1:
      indy();
      lda();
      break;
    case 0xB4:
      zpx();
      ldy();
      break;
    case 0xB5:
      zpx();
      lda();
      break;
    case 0xB6:
      zpy();
      ldx();
      break;
    case 0xB8:
      imp();
      clv();
      break;
    case 0xB9:
      absy();
      lda();
      break;
    case 0xBA:
      imp();
      tsx();
      break;
    case 0xBC:
      absx();
      ldy();
      break;
    case 0xBD:
      absx();
      lda();
      break;
    case 0xBE:
      absy();
      ldx();
      break;
    case 0xC0:
      imm();
      cpy();
      break;
    case 0xC1:
      indx();
      cmp();
      break;
    case 0xC4:
      zp();
      cpy();
      break;
    case 0xC5:
      zp();
      cmp();
      break;
    case 0xC6:
      zp();
      dec();
      break;
    case 0xC8:
      imp();
      iny();
      break;
    case 0xC9:
      imm();
      cmp();
      break;
    case 0xCA:
      imp();
      dex();
      break;
    case 0xCC:
      abso();
      cpy();
      break;
    case 0xCD:
      abso();
      cmp();
      break;
    case 0xCE:
      abso();
      dec();
      break;
    case 0xD0:
      rel();
      bne();
      break;
    case 0xD1:
      indy();
      cmp();
      break;
    case 0xD5:
      zpx();
      cmp();
      break;
    case 0xD6:
      zpx();
      dec();
      break;
    case 0xD8:
      imp();
      cld();
      break;
    case 0xD9:
      absy();
      cmp();
      break;
    case 0xDD:
      absx();
      cmp();
      break;
    case 0xDE:
      absx();
      dec();
      break;
    case 0xE0:
      imm();
      cpx();
      break;
    case 0xE1:
      indx();
      sbc();
      break;
    case 0xE4:
      zp();
      cpx();
      break;
    case 0xE5:
      zp();
      sbc();
      break;
    case 0xE6:
      zp();
      inc();
      break;
    case 0xE8:
      imp();
      inx();
      break;
    case 0xE9:
      imm();
      sbc();
      break;
    case 0xEB:
      imm();
      sbc();
      break;
    case 0xEC:
      abso();
      cpx();
      break;
    case 0xED:
      abso();
      sbc();
      break;
    case 0xEE:
      abso();
      inc();
      break;
    case 0xF0:
      rel();
      beq();
      break;
    case 0xF1:
      indy();
      sbc();
      break;
    case 0xF5:
      zpx();
      sbc();
      break;
    case 0xF6:
      zpx();
      inc();
      break;
    case 0xF8:
      imp();
      sed();
      break;
    case 0xF9:
      absy();
      sbc();
      break;
    case 0xFD:
      absx();
      sbc();
      break;
    case 0xFE:
      absx();
      inc();
      break;
  }

  instructions++;

}


uint16_t getpc() {
  return (PROGRAM_COUNTER);
}

uint8_t getop() {
  return (opcode);
}
// end of Blue6502's voids


//////////////////////////////////////////////////////////////////////////////////////////////
//
// 10_Loader.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void header_INTERNAL () {


  for (int header = 0; header < 0x7e; header++) {
    POKE (header + 0x0380, SID_data[header]);
  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void INTERNAL_LOAD() {


  for (uint16_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

    if ( LOW_RAM == true) {
      if (i < (RAM_SIZE - 0x0400) ) { //  hack to load only amount of bytes that can fit into RAM from sid file. Hopefully, player routine is the beginning of file, and tune data is in the end
        POKE (0x400 + i,  SID_data[i + 0x7e]) ;      // load .sid to RAM , relocated at 0x0400
      }
    }
    if ( LOW_RAM == false)   {
      POKE (SID_load_start + i, SID_data[i + 0x7e] );    // load .sid directly to RAM in it's exact address . // TODO: Size check
    }

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline void Loader() {



  header_INTERNAL(); //  load header to RAM at $0380 (so it can also be accessed by 6502 code)
  PLAYABLE_SID = Compatibility_check(); // set sid's globals (from RAM), true if sid is playble

  // if (PLAYABLE_SID) { // disabled for internal version.
  player_setup();
  INTERNAL_LOAD();
  reset6502(); // reset CPU
  POKE (0x0304, SID_current_tune - 1) ; // set tune number

  //  }





}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// 20_hardware.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////


// Hardware specifics, for easier porting to other microcontrollers

#ifdef USE_STM32duino_CORE
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif

//
// Set PA8 pin as PWM, at <period*F_CPU> overflow
// Set interrupt at <multiplier>


inline void InitHardware() { // setup pins and IRQ

  // init irq

  noInterrupts();

  //  pinMode(BUTTON_1, INPUT_PULLUP);

#ifdef  USE_ROGER_CORE

  pinMode (PA8, PWM); //   audio output pin

  Timer1.setPeriod(period);

  Timer2.setPrescaleFactor(1);
  Timer2.setMode(TIMER_CH2, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(multiplier);
  Timer2.setCompare(TIMER_CH2, 1);
  Timer2.attachInterrupt(TIMER_CH2, irq_handler);
#endif

#ifdef USE_STM32duino_CORE
  pinMode(PA8, OUTPUT);

  PWM->pause();
  PWM->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA8);
  PWM->setPrescaleFactor(1);
  PWM->setOverflow( period * magic_number, TICK_FORMAT);
  PWM->resume();


  HardwareTimer *IRQtimer = new HardwareTimer(TIM2);
  IRQtimer->setMode(2, TIMER_OUTPUT_COMPARE);
  IRQtimer->setOverflow(multiplier, MICROSEC_FORMAT);
  IRQtimer->attachInterrupt(irq_handler);
  IRQtimer->resume();
#endif

  interrupts();

}

inline void SetAUDIO () {

#ifdef USE_ROGER_CORE
  Timer1.setCompare(TIMER_CH1, main_volume);
#endif

#ifdef USE_STM32duino_CORE
  TIM1->CCR1 =  main_volume; //  faster version of PWM->setCaptureCompare(1, main_volume, TICK_COMPARE_FORMAT);
#endif

}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 30_tuneChecks.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////




inline void set_tune_speed () { // set tune speed best on IRQ_TYPE_PER_TUNE and VIDEO_TYPE
  SUBTUNE_SPEED = (IRQ_TYPE_PER_TUNE >> ((SID_current_tune - 1) & 0x1f)) & 0x01;
  // this need to be done per tune
  // 0 - VBI
  // 1 - CIA

  // VIDEO_TYPE
  //  00 = Unknown, //
  //  01 = PAL,
  //  10 = NTSC,
  //  11 = PAL and NTSC. // hmm, i think i'll set PAL CIA's speed for this one.

  // make it a switch to make life easier

  switch ((VIDEO_TYPE << 1) | SUBTUNE_SPEED)  {
    case 0: // 000 // VBI - UNKNOWN
      SID_speed = 20000; // assume it's pal
      break;

    case 1: // 001 // CIA - UNKNOWN
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667;
      }

      break;

    case 2: // 010 // VBI - PAL
      SID_speed = 20000;
      break;

    case 3: // 011 // CIA - PAL
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667;
      }

      break;

    case 4: // 100 // VBI - NTSC
      SID_speed = 17045;
      break;

    case 5: // 101 // CIA - NTSC
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 17095;
      }

      break;

    case 6: // 110 // VBI - PAL and NTSC
      SID_speed = 20000; // assume it's pal vbi
      break;

    case 7: // 111 // CIA - PAL and NTSC
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667; // assume it's pal cia
      }

      break;


  }

}


inline bool Compatibility_check() {

  MagicID = PEEK (0 +  0x0380) ;
  VERSION =  PEEK (0x05 +  0x0380);
  RAM_OVERFLOW = false;
  if ( ( SID_data_size + 0x0400 - 0x7e) > RAM_SIZE    ) {
    RAM_OVERFLOW = true;
  }

  LOAD_ADDRESS = 0 ;
  SID_load_start = (PEEK (0x08 +  0x0380) * 256) + (PEEK (0x09 +  0x0380));

  if (SID_load_start == 0) {
    SID_load_start = PEEK (0x7c + 0x380) + (PEEK (0x7d + 0x380) * 256);
  }
  if ( SID_load_start >= 0x07E8)  {
    LOAD_ADDRESS = SID_load_start;
  }

  SID_load_end = SID_load_start + SID_data_size - 0x7e ;
  if ( (RAM_SIZE ) < (SID_load_end ) ) {
    LOW_RAM = true;
  }
  else {
    LOW_RAM = false;
  }

  SID_play =  PEEK (0x0d + 0x0380) + (PEEK (0x0c + 0x0380) * 256);
  SID_init = PEEK (0x0b + 0x0380) + (PEEK (0x0a + 0x0380) * 256);
  SID_number_of_tunes =  PEEK (0x0f + 0x0380) + ( PEEK (0x0e + 0x0380) * 256);
  SID_default_tune =  PEEK (0x11 + 0x0380) + ( PEEK (0x10 + 0x0380) * 256);
  SID_current_tune =  SID_default_tune;



  // $12-$15 - 32bit big endian bits
  // 0 - VBI , 1 - CIA
  // for tunes > 32 , it's tune&0x1f
  // it is just indication what type of interrupts is used: VIC or CIA, and only default values are used.
  // Multispeed tune's  code set it's own VIC/CIA values. Must emulate VIC and CIA to be able to play(detect) multi speed tunes.
  IRQ_TYPE_PER_TUNE = (      ( (PEEK (0x15 + 0x0380)) )
                             |      ( (PEEK (0x14 + 0x0380)) << 8 )
                             |                          ( (PEEK (0x13 + 0x0380)) << 16 )
                             |                          ( (PEEK (0x12 + 0x0380)) << 24 )
                      );



  FLAGS76 = ( ( (PEEK (0x76 + 0x0380)) << 8 ) | (PEEK (0x77 + 0x0380)) ); // 16bit big endian number

  ComputeMUSplayer = FLAGS76 & 0x01;                        // bit0 - if set, not playable

  C64Compatible = (FLAGS76 >> 1) & 0x01;                   // bit1 - is PlaySID specific, e.g. uses PlaySID samples (psidSpecific):
  //                                                          0 = C64 compatible,                            // playable
  //                                                          1 = PlaySID specific (PSID v2NG, v3, v4)       // not playable
  //                                                          1 = C64 BASIC flag (RSID)                      // not playable

  VIDEO_TYPE =  (FLAGS76 >> 2) & 0x03 ;                       // 2bit value
  //  00 = Unknown,
  //  01 = PAL,
  //  10 = NTSC,
  //  11 = PAL and NTSC.
  // used in combination with SUBTUNE_SPEED to set speed per tune


  set_tune_speed(); // set SID_speed

  MODEL_TYPE =  (FLAGS76 >> 4) & 0x03 ;                       // SID Model
  // 00 = Unknown,
  // 01 = MOS6581,
  // 10 = MOS8580,
  // 11 = MOS6581 and MOS8580.

  //
  LOW_RAM_uncompatible = false;
  if (LOW_RAM) {
    if ( (SID_play < SID_load_start) | (SID_play > SID_load_end) ) {
      LOW_RAM_uncompatible = true;
    }
  }


  if (
    (MagicID != 0x50) /*short version */
    | (RAM_OVERFLOW)
    | (VERSION < 2)
    | (SID_play == 0)
    | (ComputeMUSplayer)
    | (C64Compatible)
    | (LOW_RAM_uncompatible) )
  { // play tune if no errors
    PLAYABLE_SID = false;
  }
  else {
    PLAYABLE_SID = true;
  }
  return PLAYABLE_SID;
}

inline void player_setup() {

  for ( temporary_variable_i = 0; temporary_variable_i < 0x80; temporary_variable_i++) {
    POKE (temporary_variable_i + 0x0300, MyROM[temporary_variable_i] ); //
  }
  POKE (0x0304, SID_current_tune - 1 );
  POKE (0x0307, (SID_init >> 8) & 0xff);
  POKE (0x0306, SID_init & 0xff);
  POKE (0x0310, (SID_play >> 8) & 0xff);
  POKE (0x030f, SID_play & 0xff);

}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 40_IRQ.ino
//
//////////////////////////////////////////////////////////////////////////////////////////////


void irq_handler(void) { //


  SetAUDIO(); // in 20_hardware.ino
  //   digitalWrite(PB13, HIGH);

    VIC_irq = VIC_irq + multiplier;
    if (VIC_irq >= SID_speed) {
      VIC_irq_request = 1;
      VIC_irq = 0;
    }



    tune_play_counter = tune_play_counter + multiplier;
    if (tune_play_counter >= tune_end_counter) {
      play_next_tune = true;
    }

    SID_emulator();

    for (int cc = 0; cc < NUMBER_OF_INSTRUCTION_PER_IRQ; cc++) {
      CPU_emulator();
    }

    if (previous_Tune != Tune) {
      play_next_tune = true;
    }


    /////////////////////////////////////////////////
    // play next tune check
    if (play_next_tune == true) { // changing subtune
      tune_play_counter = 0;

      if (SID_current_tune == SID_number_of_tunes) {
        SID_current_tune = 0;
      }

      SID_current_tune = SID_current_tune + 1;

      if (previous_Tune != Tune) {

        if ( (Tune < 1) | (Tune > SID_number_of_tunes) )  {
          Tune = SID_default_tune;
        }
        else {

          SID_current_tune = Tune  ;

        }
        previous_Tune = Tune;
      }
    reset_SID();
    set_tune_speed ();
    //infoSID();
    play_next_tune = false; // set speed and play next tune
    reset6502();
    POKE (0x0304, SID_current_tune - 1 ); // player's address for init tune

    } // play next tune check


    STAD4XX = 0;
    //   digitalWrite(PB13, LOW);
  }

  inline void CPU_emulator () {

    if (JSR1003 == 1) { // JSR1003 check
      //  digitalWrite(PB12, LOW);
      if (VIC_irq_request == 1) {
        JSR1003 = 0;
        VIC_irq_request = 0;
        //   digitalWrite(PB12, HIGH);
      }
      else {
        // tralala
      }
    }
    else {
      exec6502();
    }
  }

  inline void SID_emulator() {

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Magic
    //
    ///////////////////////////////////



   
    OSC_MSB_Previous_1 = OSC_MSB_1;
    OSC_MSB_Previous_2 = OSC_MSB_2;
    OSC_MSB_Previous_3 = OSC_MSB_3;


    OSC_1 = ((!test_bit_voice_1) & 1) * ((OSC_1 + (  multiplier * OSC_1_HiLo)) ) & 0xffffff;
    OSC_2 = ((!test_bit_voice_2) & 1) * ((OSC_2 + (  multiplier * OSC_2_HiLo)) ) & 0xffffff;
    OSC_3 = ((!test_bit_voice_3) & 1) * ((OSC_3 + (  multiplier * OSC_3_HiLo)) ) & 0xffffff;


    // noise_1
    OSC_noise_1 = OSC_noise_1 + multiplier * OSC_1_HiLo;
    OSC_bit19_1 = OSC_noise_1 >> 19 ;
    for (temporary_variable_i = 0; temporary_variable_i < OSC_bit19_1; temporary_variable_i++) {
      bit_0_1 = (( bitRead(pseudorandom_1, 22)   ) ^ ((bitRead(pseudorandom_1, 17 ) ) )  ) & 0x1;
      pseudorandom_1 = pseudorandom_1 << 1;
      //pseudorandom_1 = pseudorandom_1 & 0x7fffff;
      pseudorandom_1 = bit_0_1 | pseudorandom_1;
    }
    OSC_noise_1 = OSC_noise_1 - (OSC_bit19_1 << 19);


    // noise_2
    OSC_noise_2 = OSC_noise_2 + multiplier * OSC_2_HiLo;
    OSC_bit19_2 = OSC_noise_2 >> 19 ;
    for (temporary_variable_i = 0; temporary_variable_i < OSC_bit19_2; temporary_variable_i++) {
      bit_0_2 = (( bitRead(pseudorandom_2, 22)   ) ^ ((bitRead(pseudorandom_2, 17 ) ) )  ) & 0x1;
      pseudorandom_2 = pseudorandom_2 << 1;
      //pseudorandom_2 = pseudorandom_2 & 0x7fffff;
      pseudorandom_2 = bit_0_2 | pseudorandom_2;
    }
    OSC_noise_2 = OSC_noise_2 - (OSC_bit19_2 << 19) ;

    // noise_3
    OSC_noise_3 = OSC_noise_3 + multiplier * OSC_3_HiLo;
    OSC_bit19_3 = OSC_noise_3 >> 19 ;
    for (temporary_variable_i = 0; temporary_variable_i < OSC_bit19_3; temporary_variable_i++) {
      bit_0_3 = (( bitRead(pseudorandom_3, 22)   ) ^ ((bitRead(pseudorandom_3, 17 ) ) )  ) & 0x1;
      pseudorandom_3 = pseudorandom_3 << 1;
      //pseudorandom_3 = pseudorandom_3 & 0x7fffff;
      pseudorandom_3 = bit_0_3 | pseudorandom_3;
    }
    OSC_noise_3 = OSC_noise_3 - (OSC_bit19_3 << 19 );

    if (OSC_1 >= 0x800000)     OSC_MSB_1 = 1; else OSC_MSB_1 = 0;
    if ( (!OSC_MSB_Previous_1) & (OSC_MSB_1)) MSB_Rising_1 = 1; else  MSB_Rising_1 = 0;

    if (OSC_2 >= 0x800000)     OSC_MSB_2 = 1; else OSC_MSB_2 = 0;
    if ( (!OSC_MSB_Previous_2) & (OSC_MSB_2)) MSB_Rising_2 = 1; else  MSB_Rising_2 = 0;

    if (OSC_3 >= 0x800000)     OSC_MSB_3 = 1; else OSC_MSB_3 = 0;
    if ( (!OSC_MSB_Previous_3) & (OSC_MSB_3)) MSB_Rising_3 = 1; else MSB_Rising_3 = 0;



    if (SYNC_bit_voice_1 & MSB_Rising_3) OSC_1 = OSC_1 & 0x7fffff;
    if (SYNC_bit_voice_2 & MSB_Rising_1) OSC_2 = OSC_2 & 0x7fffff;
    if (SYNC_bit_voice_3 & MSB_Rising_2) OSC_3 = OSC_3 & 0x7fffff;

    if ( (triangle_bit_voice_1) & (ring_bit_voice_1) ) OSC_MSB_1 = OSC_MSB_1 ^ OSC_MSB_3;
    if ( (triangle_bit_voice_2) & (ring_bit_voice_2) ) OSC_MSB_2 = OSC_MSB_2 ^ OSC_MSB_1;
    if ( (triangle_bit_voice_3) & (ring_bit_voice_3) ) OSC_MSB_3 = OSC_MSB_3 ^ OSC_MSB_2;

    waveform_switch_1 = (noise_bit_voice_1 << 3) | (pulse_bit_voice_1 << 2) | (sawtooth_bit_voice_1 << 1) | (triangle_bit_voice_1);
    waveform_switch_2 = (noise_bit_voice_2 << 3) | (pulse_bit_voice_2 << 2) | (sawtooth_bit_voice_2 << 1) | (triangle_bit_voice_2);
    waveform_switch_3 = (noise_bit_voice_3 << 3) | (pulse_bit_voice_3 << 2) | (sawtooth_bit_voice_3 << 1) | (triangle_bit_voice_3);

    temp11 = (OSC_1 >> 12);

    switch (waveform_switch_1) {
      case 0:
        WaveformDA_1 = 0;
        break;
      case 1:
        WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
        WaveformDA_1 = WaveformDA_triangle_1;
        break;
      case 2:
        WaveformDA_sawtooth_1 = temp11;
        WaveformDA_1 = WaveformDA_sawtooth_1;
        break;
      case 3:
        WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
        WaveformDA_sawtooth_1 = temp11;
        WaveformDA_1 = AND_mask[(WaveformDA_triangle_1 & WaveformDA_sawtooth_1)] << 4;
        break;
      case 4:
        if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
        WaveformDA_1 = WaveformDA_pulse_1;
        break;
      case 5:
        WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
        if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
        WaveformDA_1 = AND_mask[WaveformDA_triangle_1 & WaveformDA_pulse_1] << 4;
        break;
      case 6:
        WaveformDA_sawtooth_1 = temp11; // same as upper 12 bits of OSC
        if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
        WaveformDA_1 = AND_mask[WaveformDA_sawtooth_1 & WaveformDA_pulse_1] << 4;
        break;
      case 7:
        WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
        WaveformDA_sawtooth_1 = temp11;
        if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
        WaveformDA_1 = AND_mask[WaveformDA_pulse_1 & WaveformDA_sawtooth_1 & WaveformDA_triangle_1] << 4;
        break;
      case 8:
        WaveformDA_noise_1 = B4095 & (pseudorandom_1 >> 11);

        WaveformDA_1 =  WaveformDA_noise_1;
        break;
      case 9:
        WaveformDA_1 = 0;
        break;
      case 10:
        WaveformDA_1 = 0;
        break;
      case 11:
        WaveformDA_1 = 0;
        break;
      case 12:
        WaveformDA_1 = 0;
        break;
      case 13:
        WaveformDA_1 = 0;
        break;
      case 14:
        WaveformDA_1 = 0;
        break;
      case 15:
        WaveformDA_1 = 0;
        break;

    }

    // voice 2

    temp12 = (OSC_2 >> 12);

    switch (waveform_switch_2) {
      case 0:
        WaveformDA_2 = 0;
        break;
      case 1:
        WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
        WaveformDA_2 = WaveformDA_triangle_2;
        break;
      case 2:
        WaveformDA_sawtooth_2 = temp12;
        WaveformDA_2 = WaveformDA_sawtooth_2;
        break;
      case 3:
        WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
        WaveformDA_sawtooth_2 = temp12;
        WaveformDA_2 = AND_mask[(WaveformDA_triangle_2 & WaveformDA_sawtooth_2)] << 4;
        break;
      case 4:
        if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
        WaveformDA_2 = WaveformDA_pulse_2;
        break;
      case 5:
        WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
        if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
        WaveformDA_2 = AND_mask[WaveformDA_triangle_2 & WaveformDA_pulse_2] << 4;
        break;
      case 6:
        WaveformDA_sawtooth_2 = temp12;
        if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
        WaveformDA_2 = AND_mask[WaveformDA_sawtooth_2 & WaveformDA_pulse_2] << 4;
        break;
      case 7:
        WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
        WaveformDA_sawtooth_2 = temp12;
        if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
        WaveformDA_2 = AND_mask[WaveformDA_pulse_2 & WaveformDA_sawtooth_2 & WaveformDA_triangle_2] << 4;
        break;
      case 8:
        WaveformDA_noise_2 = B4095 & (pseudorandom_2 >> 11);
        WaveformDA_2 =  WaveformDA_noise_2;
        break;
      case 9:
        WaveformDA_2 = 0;
        break;
      case 10:
        WaveformDA_2 = 0;
        break;
      case 11:
        WaveformDA_2 = 0;
        break;
      case 12:
        WaveformDA_2 = 0;
        break;
      case 13:
        WaveformDA_2 = 0;
        break;
      case 14:
        WaveformDA_2 = 0;
        break;
      case 15:
        WaveformDA_2 = 0;
        break;

    }

    // voice 3

    temp13 = (OSC_3 >> 12);

    switch (waveform_switch_3) {
      case 0:
        WaveformDA_3 = 0;
        break;
      case 1:
        WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
        WaveformDA_3 = WaveformDA_triangle_3;
        break;
      case 2:
        WaveformDA_sawtooth_3 = temp13;
        WaveformDA_3 = WaveformDA_sawtooth_3;
        break;
      case 3:
        WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
        WaveformDA_sawtooth_3 = temp13;
        WaveformDA_3 = AND_mask[(WaveformDA_triangle_3 & WaveformDA_sawtooth_3)] << 4;
        break;
      case 4:
        if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
        WaveformDA_3 = WaveformDA_pulse_3;
        break;
      case 5:
        WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
        if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
        WaveformDA_3 = AND_mask[WaveformDA_triangle_3 & WaveformDA_pulse_3] << 4;
        break;
      case 6:
        WaveformDA_sawtooth_3 = temp13;
        if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
        WaveformDA_3 = AND_mask[WaveformDA_sawtooth_3 & WaveformDA_pulse_3] << 4;
        break;
      case 7:
        WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
        WaveformDA_sawtooth_3 = temp13;
        if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
        WaveformDA_3 = AND_mask[WaveformDA_pulse_3 & WaveformDA_sawtooth_3 & WaveformDA_triangle_3] << 4;
        break;
      case 8:
        WaveformDA_noise_3 = B4095 & (pseudorandom_3 >> 11);
        WaveformDA_3 =  WaveformDA_noise_3;
        break;
      case 9:
        WaveformDA_3 = 0;
        break;
      case 10:
        WaveformDA_3 = 0;
        break;
      case 11:
        WaveformDA_3 = 0;
        break;
      case 12:
        WaveformDA_3 = 0;
        break;
      case 13:
        WaveformDA_3 = 0;
        break;
      case 14:
        WaveformDA_3 = 0;
        break;
      case 15:
        WaveformDA_3 = 0;
        break;

    }

    //ADSR1

    // gate change check

    switch (Gate_bit_1) {
      case 0:

        if (Gate_previous_1 == 1) {

          ADSR_stage_1 = 4;
          LFSR15_1 = 0;
          LFSR5_1 = 0;
          LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1];
          Gate_previous_1 = 0;
        }
        break;
      case 1:
        if (Gate_previous_1 == 0) {

          ADSR_stage_1 = 1; //
          LFSR15_1 = 0;
          LFSR5_1 = 0;
          Gate_previous_1 = 1;
          hold_zero_1 = false;
          LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Attack_1];
        }
        break;
    }

    // Increase LFSR15 counter for ADSR (scaled to match)

    LFSR15_1 = LFSR15_1 + multiplier;;
    if (   ((LFSR15_1 >= LFSR15_comparator_value_1 ) ) ) {

      Divided_LFSR15_1 = ((LFSR15_1 ) / LFSR15_comparator_value_1);
      LFSR15_1 = LFSR15_1 - Divided_LFSR15_1 * LFSR15_comparator_value_1;
      LFSR5_1 = LFSR5_1 + Divided_LFSR15_1 ;
      if ((ADSR_stage_1 == 1) | (LFSR5_1 >= LFSR5_comparator_value_1) ) {
        Divided_LFSR5_1 = (LFSR5_1 ) / LFSR5_comparator_value_1;
        if (Divided_LFSR5_1 >= 1) {
          LFSR5_1 = 0;
        }
        else {
          LFSR5_1 =  LFSR5_1 - Divided_LFSR5_1 * LFSR5_comparator_value_1;
        }

        if (hold_zero_1 == false) {
          switch (ADSR_stage_1) {
            case 0:
              break;
            case 1:
              ADSR_volume_1 = (ADSR_volume_1 + Divided_LFSR15_1) ;
              if (ADSR_volume_1 >= 0xff) {
                ADSR_volume_1 = 0xff - (ADSR_volume_1 - 0xff);
                ADSR_stage_1 = 2;
                hold_zero_1 = false;
                LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ];
              }

              break;
            case 2:
              if (ADSR_volume_1 >= Divided_LFSR5_1)        {
                ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1;
              }
              else {
                ADSR_volume_1 = 0;
              }

              if (ADSR_volume_1 <= (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {
                ADSR_volume_1 = (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1);
                LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1   ];
                ADSR_stage_1 = 3;
              }
              break;
            case 3:
              if (ADSR_volume_1 > (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {
                ADSR_stage_1 = 2;
                LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ];
              }
              break;
            case 4:
              if (ADSR_volume_1 >= Divided_LFSR5_1)        {
                ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1;
              }
              else {
                ADSR_volume_1 = 0;
              }
              break;

          }

          LFSR5_comparator_value_1 = ADSR_Volume2LFSR5[ADSR_volume_1];


          if (ADSR_volume_1 == 0) {
            hold_zero_1 = true;
          }

        }
      }




    }

    //ADSR2

    // gate change check

    switch (Gate_bit_2) {
      case 0:

        if (Gate_previous_2 == 1) {
          ADSR_stage_2 = 4;
          LFSR15_2 = 0;
          LFSR5_2 = 0;
          LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2];
          Gate_previous_2 = 0;


        }



        break;
      case 1:
        if (Gate_previous_2 == 0) {
          ADSR_stage_2 = 1;
          LFSR15_2 = 0;
          LFSR5_2 = 0;
          Gate_previous_2 = 1;
          hold_zero_2 = false;
          LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Attack_2];
        }
        break;
    }

    // Increase LFSR15 counter for ADSR (scaled to match)

    LFSR15_2 = LFSR15_2 + multiplier;;
    if (   ((LFSR15_2 >= LFSR15_comparator_value_2 ) ) ) {
      Divided_LFSR15_2 = ((LFSR15_2 ) / LFSR15_comparator_value_2);
      LFSR15_2 = LFSR15_2 - Divided_LFSR15_2 * LFSR15_comparator_value_2;
      LFSR5_2 = LFSR5_2 + Divided_LFSR15_2 ;
      if ((ADSR_stage_2 == 1) | (LFSR5_2 >= LFSR5_comparator_value_2) ) {
        Divided_LFSR5_2 = (LFSR5_2 ) / LFSR5_comparator_value_2;
        if (Divided_LFSR5_2 >= 1) {
          LFSR5_2 = 0;
        }
        if (hold_zero_2 == false) {
          switch (ADSR_stage_2) {

            case 0:

              break;
            case 1:

              ADSR_volume_2 = (ADSR_volume_2 + Divided_LFSR15_2) ;

              if (ADSR_volume_2 >= 0xff) {
                ADSR_volume_2 = 0xff - (ADSR_volume_2 - 0xff);
                ADSR_stage_2 = 2;
                hold_zero_2 = false;
                LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ];
              }
              break;
            case 2:
              if (ADSR_volume_2 >= Divided_LFSR5_2)        {
                ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2;
              }
              else {
                ADSR_volume_2 = 0;
              }
              if (ADSR_volume_2 <= (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
                ADSR_volume_2 = (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2);
                LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2   ];
                ADSR_stage_2 = 3;
              }
              break;
            case 3:
              if (ADSR_volume_2 > (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
                ADSR_stage_2 = 2;
                LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ];
              }
              break;
            case 4:
              if (ADSR_volume_2 >= Divided_LFSR5_2)        {
                ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2;
              }
              else {
                ADSR_volume_2 = 0;
              }
              break;
          }

          LFSR5_comparator_value_2 = ADSR_Volume2LFSR5[ADSR_volume_2];

          if (ADSR_volume_2 == 0) {
            hold_zero_2 = true;
          }
        }
      }
    }

    //ADSR3

    // gate change check

    switch (Gate_bit_3) {
      case 0:

        if (Gate_previous_3 == 1) {
          ADSR_stage_3 = 4;
          LFSR15_3 = 0;
          LFSR5_3 = 0;
          LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3];
          Gate_previous_3 = 0;
        }
        break;
      case 1:
        if (Gate_previous_3 == 0) {
          ADSR_stage_3 = 1; //
          LFSR15_3 = 0;
          LFSR5_3 = 0;
          Gate_previous_3 = 1;
          hold_zero_3 = false;
          LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Attack_3];
        }
        break;

    }

    // Increase LFSR15 counter for ADSR (scaled to match)

    LFSR15_3 = LFSR15_3 + multiplier;;
    if (   ((LFSR15_3 >= LFSR15_comparator_value_3 ) ) ) {

      Divided_LFSR15_3 = ((LFSR15_3 ) / LFSR15_comparator_value_3);
      LFSR15_3 = LFSR15_3 - Divided_LFSR15_3 * LFSR15_comparator_value_3;
      LFSR5_3 = LFSR5_3 + Divided_LFSR15_3 ;

      if ((ADSR_stage_3 == 1) | (LFSR5_3 >= LFSR5_comparator_value_3) ) {
        Divided_LFSR5_3 = (LFSR5_3 ) / LFSR5_comparator_value_3;
        if (Divided_LFSR5_3 >= 1) {
          LFSR5_3 = 0;
        }
        else {
          LFSR5_3 =  LFSR5_3 - Divided_LFSR5_3 * LFSR5_comparator_value_3;
        }
        if (hold_zero_3 == false) {

          switch (ADSR_stage_3) {
            case 0:
              break;
            case 1:
              ADSR_volume_3 = (ADSR_volume_3 + Divided_LFSR15_3) ;
              if (ADSR_volume_3 >= 0xff) {
                ADSR_volume_3 = 0xff - (ADSR_volume_3 - 0xff);
                ADSR_stage_3 = 2;
                hold_zero_3 = false;
                LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ];
              }
              break;
            case 2:
              if (ADSR_volume_3 >= Divided_LFSR5_3)        {
                ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3;
              }
              else {
                ADSR_volume_3 = 0;
              }
              if (ADSR_volume_3 <= (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
                ADSR_volume_3 = (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3);
                LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3   ];
                ADSR_stage_3 = 3;
              }
              break;
            case 3:
              if (ADSR_volume_3 > (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
                ADSR_stage_3 = 2;
                LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ];
              }
              break;
            case 4:
              if (ADSR_volume_3 >= Divided_LFSR5_3)        {
                ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3;
              }
              else {
                ADSR_volume_3 = 0;
              }
              break;
          }

          LFSR5_comparator_value_3 = ADSR_Volume2LFSR5[ADSR_volume_3];

          if (ADSR_volume_3 == 0) {
            hold_zero_3 = true;
          }

        }
      }
    }

    //   FILTERS:


#ifndef USE_FILTERS
    if (FILTER_Enable_1) {
      if (FILTER_LP) {
        WaveformDA_1 = 0xfff - WaveformDA_1  ;
      }
      if (FILTER_HP) {
        WaveformDA_1 = 0xfff - WaveformDA_1 ;
      }
    }
    if (FILTER_Enable_2) {
      if (FILTER_LP) {
        WaveformDA_2 = 0xfff - WaveformDA_2 ;
      }
      if (FILTER_HP) {
        WaveformDA_2 = 0xfff - WaveformDA_2 ;
      }
    }
    if (FILTER_Enable_3) {
      if (FILTER_LP) {
        WaveformDA_3 = 0xfff - WaveformDA_3 ;
      }
      if (FILTER_HP) {
        WaveformDA_3 = 0xfff - WaveformDA_3 ;
      }
    }

#endif

#ifndef USE_CHANNEL_1
    WaveformDA_1 = 0;
#endif
#ifndef USE_CHANNEL_2
    WaveformDA_2 = 0;
#endif
#ifndef USE_CHANNEL_3
    WaveformDA_3 = 0;
#endif

    /////////////////// ANALOG ///////////////////////////

    Volume_1 = int32_t(WaveformDA_1 * ADSR_volume_1) - 0x80000 ;
    Volume_2 = int32_t(WaveformDA_2 * ADSR_volume_2) - 0x80000 ;
    Volume_3 = int32_t(WaveformDA_3 * ADSR_volume_3) - 0x80000 ;

    // FILTERS redirect to filtered or unfiltered output

    switch (FILTER_Enable_switch) {
      default:
      case 0x0:
        Volume_filtered = 0;
        if (OFF3 )
        {
          Volume_unfiltered = Volume_1 + Volume_2;
        }
        else {
          Volume_unfiltered = Volume_1 + Volume_2 + Volume_3 ;
        }
        break;
      case 0x1:
        Volume_filtered = Volume_1;
        if (OFF3 )
        {
          Volume_unfiltered =  Volume_2;
        }
        else
        {
          Volume_unfiltered = Volume_2 + Volume_3 ;
        }
        break;
      case 0x2:
        Volume_filtered = Volume_2;
        if (OFF3 )
        {
          Volume_unfiltered = Volume_1  ;
        }
        else
        {
          Volume_unfiltered = Volume_1 + Volume_3 ;
        }
        break;
      case 0x3:
        Volume_filtered = Volume_1 + Volume_2;
        if (OFF3 )
        {
          Volume_unfiltered = 0 ;
        }
        else
        {
          Volume_unfiltered = Volume_3 ;
        }
        break;
      case 0x4:
        Volume_filtered = Volume_3;
        Volume_unfiltered = Volume_1 + Volume_2 ;
        break;
      case 0x5:
        Volume_filtered = Volume_1 + Volume_3;
        Volume_unfiltered = Volume_2 ;
        break;
      case 0x6:
        Volume_filtered = Volume_2 + Volume_3;
        Volume_unfiltered = Volume_1 ;
        break;
      case 0x7:
        Volume_filtered = Volume_1 + Volume_2 + Volume_3;
        Volume_unfiltered = 0;
        break;
    }

    Volume_filter_input = Volume_filtered;
    Volume_filter_output = Volume_filtered;


#ifdef USE_FILTERS

    Volume_filter_input = int32_t(Volume_filter_input) >> 7;

    delta_t = multiplier;
    delta_t_flt = FILTER_SENSITIVITY;

    while (delta_t) {
      if (delta_t < delta_t_flt) {
        delta_t_flt = delta_t;
      }

      w0_delta_t = (int32_t(w0_ceil_dt * delta_t_flt) >> 6);
      dVbp = (int32_t(w0_delta_t*Vhp) >> 14);
      dVlp = (int32_t(w0_delta_t*Vbp) >> 14);
      Vbp -= dVbp;
      Vlp -= dVlp;
      Vhp = (int32_t(Vbp * (Q_1024_div)) >> 10) - Vlp - Volume_filter_input;
      delta_t -= delta_t_flt;
    }

    Volume_filter_output = 0;
    if (FILTER_LP) {
      Volume_filter_output = Volume_filter_output + Vlp;
    }
    if (FILTER_HP) {
      Volume_filter_output = Volume_filter_output + Vhp;
    }
    if (FILTER_BP) {
      Volume_filter_output = Volume_filter_output + Vbp;
    }
    //

    Volume_filter_output = (int32_t(Volume_filter_output) << 7);

#endif

    Volume = ((Volume_filter_output + Volume_unfiltered) >> 2 ) + 0x80000;

    if (Volume < 0) Volume = 0;
    if (Volume > 0xfffff) Volume = 0xfffff;

    // main_volume_32bit = ( magic_number * period * ((Volume)&0xfffff) * MASTER_VOLUME) >> 24;
    main_volume_32bit = (Volume ) ;
    main_volume_32bit = (main_volume_32bit * magic_number);
    main_volume_32bit = (main_volume_32bit) >> 12;
    main_volume_32bit = (main_volume_32bit *  MASTER_VOLUME);
    main_volume_32bit = (main_volume_32bit * period) ;
    main_volume_32bit = (main_volume_32bit ) >> 12;
    main_volume = main_volume_32bit + 1;

    OSC3 =  (OSC_3 >> 16) & 0xff; //
    /*
      OSC3 =  (((OSC_3 & 0x400000) >> 11) | // OSC3 output for SID register
               ((OSC_3 & 0x100000) >> 10) |
               ((OSC_3 & 0x010000) >> 7) |
               ((OSC_3 & 0x002000) >> 5) |
               ((OSC_3 & 0x000800) >> 4) |
               ((OSC_3 & 0x000080) >> 1) |
               ((OSC_3 & 0x000010) << 1) |
               ((OSC_3 & 0x000004) << 2) )&0xff;
    */
    //
    ENV3 = (ADSR_volume_3) & 0xff; ; // ((Volume_3 + 0x80000) >> 12) & 0xff;
    //
    SID[25] = POTX;
    SID[26] = POTY;
    SID[27] = OSC3;
    SID[28] = ENV3;



  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 50_autoconfig.ino
  //
  //////////////////////////////////////////////////////////////////////////////////////////////

  uint32_t Real_uS_start;
  uint32_t Real_uS_end;
  uint32_t Emu_uS = 0;
  uint32_t Real_uS = 0 ;
  uint32_t CPU_uS = 0;
  uint32_t SID_uS = 0;
  uint32_t best_multiplier = 1;
  uint32_t estimated_frame_time = 0;
  uint32_t test_instructions = 1000;              // number of instructions to test

  //CPU_test();                                   // calculate how well 6502 can be emulated
  //autoconfigMultiplier();                       // calculate how long IRQ will last based on chosen microcontroller
  //benchmark();                                  // SID emulation + 6502 CPU emulation debug
  //FRAMEtest();                                  // calculate time of one emulated frame (SID emulation is ON)
  //HELP();                                       // if available free bytes , it can be added to RAM_SIZE




  inline void CPU_test () {

    // needed to get CPU_us
    //debugPrintTXTln("");
    //debugPrintTXTln("CPU test ");

    // calculate how well 6502 can be emulated
    // CPU_test
    reset6502(); //

    Emu_uS = 0;
    Real_uS = 0;
    instructions = 0;
    Real_uS_start = micros();
    for (uint32_t total_instructions = 0; total_instructions < test_instructions; total_instructions++) {

      exec6502(); // execute 1 instruction
      Emu_uS = Emu_uS + (ticktable[opcode]);
    }// instrucion loop
    Real_uS_end = micros();
    Emu_uS = Emu_uS;
    Real_uS = Real_uS_end - Real_uS_start;
    CPU_uS = Real_uS / test_instructions;


    debugPrintTXTln ("");
    debugPrintTXT   ("Microcontroller speed:");  debugPrintNUMBER(uint32_t(F_CPU / 1000000));  debugPrintTXTln (" MHz");
    //debugPrintTXTln (" ");
    //debugPrintTXTln ("6502 emulation only:");
    //debugPrintTXT   ("Instructions executed:               ");
    //debugPrintNUMBER(instructions);
    //debugPrintTXTln  (" ");
    //debugPrintTXT   ("Real time passed:                    ");
    //debugPrintNUMBER(Real_uS);
    //debugPrintTXTln (" uS");
    //debugPrintTXT   ("Emulated time passed:                ");
    //debugPrintNUMBER(Emu_uS);
    //debugPrintTXTln (" uS");
    debugPrintTXT   ("Emulated 6502 speed:");  debugPrintNUMBER(uint32_t((100 * Emu_uS) / Real_uS ));  debugPrintTXTln ("%");
    debugPrintTXT   ("6502 instruction:   ");
    debugPrintNUMBER(CPU_uS );
    debugPrintTXTln (" uS");


  }

  inline void autoconfigMultiplier () {
    // calculate how long IRQ will last based on chosen microcontroller

    Emu_uS = 0;
    Real_uS = 0;

    multiplier = 1;
    best_multiplier = 1;

    while (best_multiplier == 1) { //

      Real_uS_start = micros();
      for (uint32_t total_instructions = 0; total_instructions < test_instructions; total_instructions++) {
        SID_emulator();
      }
      Real_uS_end = micros();
      Real_uS = Real_uS_end - Real_uS_start;
      SID_uS = Real_uS / test_instructions;

      Total_IRQ_uS = NUMBER_OF_INSTRUCTION_PER_IRQ * CPU_uS + SID_uS + GENEROUS + 0 /* irq overhead */;


      //    estimated_frame_time = ((20000 /* ms */  / multiplier) * (SID_uS + 2 /* irq overhead */ )) + (500 /* estimated irq instructions */ * (CPU_uS + 0 /* estimated main loop overhead in uS */  )); // estimated around 500 instruction per frame (20mS) should be enough for calculations

      estimated_frame_time = ((20000 /* ms */  / multiplier) * (  Total_IRQ_uS  )) ;




      // enable this if raw values are needed during calculations (in case to manualy find perfect <multiplier> value )

      /*
        debugPrintTXT("SID emulation: ");
        debugPrintNUMBER(Real_uS);
        debugPrintTXT(" uS passed. ");
        debugPrintTXT("Around: ");
        debugPrintNUMBER(Total_IRQ_uS);
        debugPrintTXT(" uS per irq, ");
        debugPrintTXT("at multiplier: ");
        debugPrintNUMBER(multiplier);
        debugPrintTXT(". Estimated emulated IRQ time: ");
        debugPrintNUMBER(estimated_frame_time);
        debugPrintTXTln("uS.");
      */

      if (estimated_frame_time < 13000) { // worst case: 13000 for bluepill at 48MHz, O0 (smallest code) optimatization (best case is same as emulated uS)
        best_multiplier = multiplier ;
      }

      multiplier++;
      if (multiplier > 248) {
        multiplier = 248;
        best_multiplier = multiplier; // if it's not found by now, then take worst case.
      }
    }
    multiplier = best_multiplier; // or edit manually here

    debugPrintTXT    ("SID emulator IRQ:  ");  debugPrintNUMBER (SID_uS);  debugPrintTXTln  (" uS ");
    debugPrintTXT    ("Estimated IRQ time:");  debugPrintNUMBER (Total_IRQ_uS);  debugPrintTXTln  (" uS ");
    debugPrintTXT    ("Optimal multiplier:");  debugPrintNUMBER (multiplier);  debugPrintTXTln  (" (uS) ");
    debugPrintTXT    ("Samplerate:        ");  debugPrintNUMBER (uint32_t(1000000 / multiplier));  debugPrintTXTln  (" Hz ");

    if (multiplier < 12) {
      period = multiplier;
    }
    else {
      period = 4; // for now, , maybe i'll leave at starting value (but it must be less then multiplier)
    }

  }

  /*
    inline void FRAMEtest () {
      // calculate time of one emulated frame (SID emulation is ON)
      reset6502();

      while (JSR1003 == 0) { // skip first JSR$1003
        exec6502();
      }

      Emu_uS = 0;
      Real_uS = 0;
      instructions = 0;
      Real_uS_start = micros();

      JSR1003 = 0;
      while (JSR1003 == 0) { // now do 1 frame to measure timing

        exec6502(); // execute 1 instruction
        Emu_uS = Emu_uS + (ticktable[opcode]);

      }

      Real_uS_end = micros();
      Real_uS = Real_uS_end - Real_uS_start;

      debugPrintTXTln  ("");
      debugPrintTXTln  ("Frame Test");
      debugPrintTXT    ("Instructions executed: ");  debugPrintNUMBER (instructions);  debugPrintTXTln  ("");
      debugPrintTXT    ("Frame time (real):     ");  debugPrintNUMBER (uint32_t(Real_uS));  debugPrintTXTln  ("uS ");
      debugPrintTXT    ("Frame time (emulated): ");  debugPrintNUMBER (uint32_t(Emu_uS));  debugPrintTXTln  ("uS ");
      debugPrintTXT    ("Emulated frame speed:  ");  debugPrintNUMBER (uint32_t((100 * Emu_uS) / Real_uS ));  debugPrintTXTln  ("%");
      Emu_uS = 0;
      Real_uS = 0;

    }
  */
  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 60_barebone_sounds.ino
  //
  //////////////////////////////////////////////////////////////////////////////////////////////


  void error_sound_SD() {

    reset_SID();

    OSC_1_HiLo = 0xffff; // just having fun with globals here :-)
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x09;
    ADSR_Decay_1  = 0x07;
    ADSR_Sustain_1 = 0x06;
    ADSR_Release_1 = 0x0b;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    triangle_bit_voice_1 = 1;
    pulse_bit_voice_1 = 1;
    Gate_bit_1 = 1;
    delay(480);
    OSC_1_HiLo = 0xf000;
    Gate_bit_1 = 0;
    delay(4000);
  }



  inline void error_sound_ROOT() {

    reset_SID();

    OSC_1_HiLo = 0x1000; // just having fun with globals here :-)
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x09;
    ADSR_Decay_1  = 0x07;
    ADSR_Sustain_1 = 0x06;
    ADSR_Release_1 = 0x0b;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    triangle_bit_voice_1 = 1;
    pulse_bit_voice_1 = 1;
    Gate_bit_1 = 1;
    delay(480);
    OSC_1_HiLo = 0x0800;
    Gate_bit_1 = 0;
    delay(1000);

  }


  inline void error_open_file() {

    reset_SID();

    OSC_1_HiLo = 0xc000;
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x00;
    ADSR_Decay_1  = 0x00;
    ADSR_Sustain_1 = 0x0f;
    ADSR_Release_1 = 0x05;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    // triangle_bit_voice_1 = 1;
    //pulse_bit_voice_1 = 1;
    noise_bit_voice_1;
    Gate_bit_1 = 1;
    delay(480);
    OSC_1_HiLo = 0xc800;
    Gate_bit_1 = 0;
    delay(480);

  }


  inline void error_open_folder () {

    reset_SID();

    OSC_1_HiLo = 0x2000; // just having fun with globals here :-)
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x09;
    ADSR_Decay_1  = 0x07;
    ADSR_Sustain_1 = 0x06;
    ADSR_Release_1 = 0x0b;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    triangle_bit_voice_1 = 1;
    pulse_bit_voice_1 = 1;
    Gate_bit_1 = 1;
    delay(480);
    OSC_1_HiLo = 0x1000;
    Gate_bit_1 = 0;

    delay(1000);
  }

  inline void error_open_sid () {

    reset_SID();

    OSC_1_HiLo = 0x4000;
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x0b;
    ADSR_Decay_1  = 0x08;
    ADSR_Sustain_1 = 0x06;
    ADSR_Release_1 = 0x0b;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    triangle_bit_voice_1 = 1;
    pulse_bit_voice_1 = 1;
    Gate_bit_1 = 1;
    delay(1500);
    OSC_1_HiLo = 0x3f00;
    Gate_bit_1 = 0;
    Gate_bit_2 = 0;
    Gate_bit_3 = 0;
    for (int oscup = 0; oscup > 4000; oscup++) {
      OSC_1_HiLo = oscup;
      delay(1);
    }
  }

  inline void error_PSID_V2_RAM_OVERFLOW () {

    reset_SID();

    OSC_1_HiLo = 0x4000; // barebone sound
    MASTER_VOLUME  = 0x0f;
    ADSR_Attack_1  = 0x0b;
    ADSR_Decay_1  = 0x08;
    ADSR_Sustain_1 = 0x06;
    ADSR_Release_1 = 0x0b;
    PW_HiLo_voice_1 = 0x400;
    //sawtooth_bit_voice_1=1;
    triangle_bit_voice_1 = 1;
    pulse_bit_voice_1 = 1;
    Gate_bit_1 = 1;
    delay(1500);
    OSC_1_HiLo = 0x3f00;
    Gate_bit_1 = 0;
    Gate_bit_2 = 0;
    Gate_bit_3 = 0;

    for (int oscdown = 4000; oscdown > 0; oscdown = oscdown - 2) {
      OSC_1_HiLo = oscdown;
      delay(1);
    }
  }

  inline void reset_SID() {


    OSC_1_HiLo = 0;
    PW_HiLo_voice_1 = 0;
    noise_bit_voice_1 = 0;
    pulse_bit_voice_1 = 0;
    sawtooth_bit_voice_1 = 0;
    triangle_bit_voice_1 = 0;
    test_bit_voice_1   = 0;
    ring_bit_voice_1   = 0;
    SYNC_bit_voice_1 = 0;
    Gate_bit_1 = 0;
    ADSR_Attack_1 = 0;
    ADSR_Decay_1 = 0;
    ADSR_Sustain_1 = 0;
    ADSR_Release_1 = 0;
    OSC_2_HiLo = 0;
    OSC_2_HiLo = 0;
    PW_HiLo_voice_2 = 0;
    noise_bit_voice_2 = 0;
    pulse_bit_voice_2 = 0;
    sawtooth_bit_voice_2 = 0;
    triangle_bit_voice_2 = 0;
    test_bit_voice_2   = 0;
    ring_bit_voice_2 = 0;
    SYNC_bit_voice_2 = 0;
    Gate_bit_2 = 0;
    ADSR_Attack_2 = 0;
    ADSR_Decay_2 = 0;
    ADSR_Sustain_2 = 0;
    ADSR_Release_2 = 0;
    OSC_3_HiLo = 0;
    PW_HiLo_voice_3 = 0;
    noise_bit_voice_3 = 0;
    pulse_bit_voice_3 = 0;
    sawtooth_bit_voice_3 = 0;
    triangle_bit_voice_3 = 0;
    test_bit_voice_3  = 0;
    ring_bit_voice_3 = 0;
    SYNC_bit_voice_3 = 0;
    Gate_bit_3 = 0;
    ADSR_Attack_3 = 0;
    ADSR_Decay_3 = 0;
    ADSR_Sustain_3 = 0;
    ADSR_Release_3 = 0;
    FILTER_HiLo = 0;
    FILTER_Resonance = 0;
    FILTER_Enable_1 =  0;
    FILTER_Enable_2 = 0;
    FILTER_Enable_3 = 0;
    FILTER_Enable_EXT = 0;
    FILTER_Enable_switch = 0;
    OFF3 =  0;
    FILTER_HP =  0;
    FILTER_BP =  0;
    FILTER_LP =  0;
    MASTER_VOLUME =   0;

  }



  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 70_SIDinfo.ino
  //
  //////////////////////////////////////////////////////////////////////////////////////////////


  char  SIDinfo_filetype [8] ;
  char  SIDinfo_name [33] ;
  char  SIDinfo_author [33] ;
  char  SIDinfo_released [33] ;
  char  SIDinfo_VIDEO [14] ;
  char  SIDinfo_CLOCK [7] ;
  char  SIDinfo_PLAYABLE [6];
  char  SIDinfo_MODEL [20];
  char  SIDinfo_PLAYLIST [10];
  char  SIDinfo_RANDOM [4];

  void  infoSID() {

    // - PSID (0x50534944)
    // - RSID (0x52534944)

    strcpy (SIDinfo_filetype, "UNKNOWN"); // if PSID/RSID check fail

    if ( (PEEK (0x380 + 00) == 0x50) & (PEEK (0x380 + 01) == 0x53) & (PEEK (0x380 + 02) == 0x49) & (PEEK (0x380 + 03) == 0x44) ) {
      strcpy (SIDinfo_filetype, "PSID");
    }

    if ( (PEEK (0x380 + 00) == 0x52) & (PEEK (0x380 + 01) == 0x53) & (PEEK (0x380 + 02) == 0x49) & (PEEK (0x380 + 03) == 0x44) ) {
      strcpy (SIDinfo_filetype, "RSID");
    }

    strcpy (SIDinfo_name, "");
    for (int cc = 0; cc < 0x20; cc = cc + 1) {
      SIDinfo_name[cc] = (PEEK (0x380 + 0x16 + cc));
      if (cc == 0x1f) {
        SIDinfo_name[0x20] = 0; // null terminating string
      }
    }
    strcpy (SIDinfo_author, "");
    for (int cc = 0; cc < 0x20; cc = cc + 1) {
      SIDinfo_author[cc] = (PEEK (0x380 + 0x36 + cc));
      if (cc == 0x1f) {
        SIDinfo_author[0x20] = 0; // null terminating string
      }
    }
    strcpy (SIDinfo_released, "");
    for (int cc = 0; cc < 0x20; cc = cc + 1) {
      SIDinfo_released[cc] = (PEEK (0x380 + 0x56 + cc));
      if (cc == 0x1f) {
        SIDinfo_released[0x20] = 0; // null terminating string
      }
    }


    switch (VIDEO_TYPE) {
      case 0:
        strcpy (SIDinfo_VIDEO, "UNKNOWN");
        break;
      case 1:
        strcpy (SIDinfo_VIDEO, "PAL");
        break;
      case 2:
        strcpy (SIDinfo_VIDEO, "NTSC");
        break;
      case 3:
        strcpy (SIDinfo_VIDEO, "PAL and NTSC");
        break;

    }
    switch (SUBTUNE_SPEED) {
      case 0:
        strcpy (SIDinfo_CLOCK, "VIC II");
        break;
      case 1:
        strcpy (SIDinfo_CLOCK, "CIA");
        break;
    }

    if (PLAYABLE_SID) {
      strcpy (SIDinfo_PLAYABLE, "OK");
    } else {
      strcpy (SIDinfo_PLAYABLE, "ERROR");
    }

    switch (MODEL_TYPE) {
      case 0:
        strcpy (SIDinfo_MODEL, "UNKNOWN");
        break;
      case 1:
        strcpy (SIDinfo_MODEL, "MOS6581");
        break;
      case 2:
        strcpy (SIDinfo_MODEL, "MOS8580");
        break;
      case 3:
        strcpy (SIDinfo_MODEL, "MOS6581 and MOS8580");
        break;
    }
    /*
      if (favorites_finished) {
        strcpy (SIDinfo_PLAYLIST, "HVSC");
      }
      else {
        strcpy (SIDinfo_PLAYLIST, "FAVORITES");
      }
    */
    /*
      if (RANDOM_FOLDERS) {
        strcpy (SIDinfo_RANDOM, "ON");

      }
      else {
        strcpy (SIDinfo_RANDOM, "OFF");
      }
    */

    debugPrintTXTln ("");
    //  debugPrintTXTln ("--------------------------------------------------------------------");
    //  debugPrintTXT   (">"); debugPrintTXT  (SID_DIR_name); debugPrintTXTln ("");
    //  debugPrintTXT   (">"); debugPrintTXT  (SID_filename); debugPrintTXTln ("");
    debugPrintTXTln ("--------------------------------------------------------------------");
    debugPrintTXT   ("Player:    "); debugPrintTXT  (SIDinfo_PLAYABLE); debugPrintTXTln ("");
    debugPrintTXT   ("Size:      "); debugPrintNUMBER(SID_data_size); debugPrintTXT(" bytes"); debugPrintTXTln (" ");
    debugPrintTXT   ("Type:      "); debugPrintTXT (SIDinfo_filetype); debugPrintTXTln ("");
    debugPrintTXT   ("Name:      "); debugPrintTXT (SIDinfo_name); debugPrintTXTln ("");
    debugPrintTXT   ("Author:    "); debugPrintTXT (SIDinfo_author); debugPrintTXTln ("");
    debugPrintTXT   ("Released:  "); debugPrintTXT (SIDinfo_released); debugPrintTXTln ("");
    debugPrintTXT   ("Range:     "); debugPrintNUMBERHEX(SID_load_start); debugPrintTXT(" - "); debugPrintNUMBERHEX(SID_load_end); debugPrintTXTln ("");
    delay(1);
    debugPrintTXT   ("Init:      "); debugPrintNUMBERHEX(SID_init); debugPrintTXTln ("");
    debugPrintTXT   ("Play:      "); debugPrintNUMBERHEX(SID_play); debugPrintTXTln ("");
    debugPrintTXT   ("Video:     "); debugPrintTXT   (SIDinfo_VIDEO); debugPrintTXTln ("");
    debugPrintTXT   ("Clock:     "); debugPrintTXT   (SIDinfo_CLOCK); debugPrintTXTln ("");
    debugPrintTXT   ("SID model: "); debugPrintTXT   (SIDinfo_MODEL); debugPrintTXTln ("");
    //  debugPrintTXT   ("Playlist:  "); debugPrintTXT   (SIDinfo_PLAYLIST); debugPrintTXTln ("");
    //  debugPrintTXT   ("Random   : "); debugPrintTXT   (SIDinfo_RANDOM); debugPrintTXTln ("");
    debugPrintTXT   ("Tune:      "); debugPrintNUMBER(SID_current_tune); debugPrintTXT   ("/"); debugPrintNUMBER(SID_number_of_tunes); debugPrintTXT   (" ("); debugPrintNUMBER(SID_default_tune); debugPrintTXT   (")") ; debugPrintTXTln ("");
    debugPrintTXTln ("");
  }


  // calculate free unused RAM
  void HELP () {
    debugPrintTXTln  ("");
    debugPrintTXT    ("  **** STM32 SID PLAYER ****    ");
    debugPrintTXTln  ("");
    debugPrintNUMBER ((RAM_SIZE >> 10) + 1);
    debugPrintTXT    ("K RAM SYSTEM  ");
    debugPrintNUMBER (FreeBytes());
    debugPrintTXT    (" BYTES FREE");
    debugPrintTXTln  ("");
    debugPrintNUMBER (uint32_t(1000000 / multiplier));
    debugPrintTXT    ("Hz Sample ");
    debugPrintNUMBER (uint32_t( (1000000 / period) / 1000));
    debugPrintTXTln  ("KHz PWM frequency");
    debugPrintTXTln  ("");

    debugPrintTXTln  ("- 1 click  - next tune");
    debugPrintTXTln  ("- 2 clicks - next tune");
    debugPrintTXTln  ("- 3 clicks - next tune");
    debugPrintTXTln  ("- 4 clicks - show HELP");
    debugPrintTXTln  ("- 5 clicks - SID info ");
    //  debugPrintTXTln  ("- 6 clicks - switch  playlist folder ( FAVORITE/HVSC ) ");
    //  debugPrintTXTln  ("- 7 clicks - random ON/OFF");
    debugPrintTXTln  ("- holding  - fast forward     (play tune as fast as possible)");


    debugPrintTXTln  ("");
    debugPrintTXTln  ("READY.");
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 80_buttons.ino
  //
  //////////////////////////////////////////////////////////////////////////////////////////////

  /*
    // 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed
    //                                             - 1 short click  - play next tune
    //                                             - 2 short clicks - play next tune
    //                                             - 3 short clicks - play next tune
    //                                             - 4 short clicks - show HELP (on any output defined)
    //                                             - 5 short clicks - show info about sid file (on any output defined)
    //                                             - 6 short clicks -
    //                                             - 7 short clicks -
    //                                             - button holding - play tune as fast as possible (fast forward)
    int16_t buttonState;
    uint32_t BT_time_start;
    uint32_t BT_time_end;
    uint32_t BT_previous_time;
    uint32_t BT_curent_time;
    int16_t state = HIGH;
    int16_t prevState = HIGH;
    int16_t clickCount = 0;
    int buttonValue = 0;


    uint32_t BT_debounce_time = 100;
    uint32_t BT_short_click_time = 650;
    uint32_t BT_is_pressed_time = 1500;
    uint32_t BT_short_click_lag_time = 333;

    int GetButtonStatus() {

    state = digitalRead(BUTTON_1);
    BT_curent_time = millis();

    if (state != prevState) { // debounce
      if ( (BT_curent_time - BT_previous_time) > BT_debounce_time) {
        BT_previous_time = BT_curent_time;
      }
      else {
        state = prevState;
      }
    }

    if ( prevState == HIGH && state == HIGH   ) {

      if (  ((BT_curent_time - BT_time_end) > BT_short_click_lag_time )  ) { // button is "sensed" after timing out, so it have little "lag"
        buttonValue = clickCount;
        clickCount = 0; // reset clickCount if short click timed out
      }
    }

    if ( prevState == HIGH  && state == LOW) { // button just pressed, just memorize time of a event
      BT_time_start = BT_curent_time;
      prevState = state;
    }

    if (prevState == LOW && state == LOW   ) {
      prevState = state;
      if (BT_curent_time > (BT_time_start + BT_is_pressed_time)) {
        clickCount = 0; //
        buttonValue =  -1; // is_pressed
      }
    }

    if ( prevState == LOW  && state == HIGH ) { // button released
      BT_time_end = BT_curent_time;
      prevState = state;
      if (  ((BT_time_end - BT_time_start) < BT_short_click_time)  ) {
        clickCount++;
        buttonValue = 0; //                                                            will keep counting clicks
      }

      if (((BT_time_end - BT_time_start)  > BT_is_pressed_time) ) {
        clickCount = 0;
        buttonValue = 0; //                                                             it was is_pressed so far, ignore release
      }
    }



    return buttonValue;
    }

    void checkButton1() {

    switch ( GetButtonStatus() ) {

      case 0:
        // tralalala
        break;
      case -1:
        // 3s long press and holding - play tune as fast as possible (fast forward)
        VIC_irq_request = 1;
        break;

      case 1:
        // 1 short click - play next tune
        play_next_tune = true;
        tune_play_counter = 0;
        delay(100);
        break;

      case 2:
        // two clicks - play next tune
        play_next_tune = true;
        tune_play_counter = 0;
        delay(100);
        break;

      case 3:
        // 3 clicks - play next tune
        play_next_tune = true;
        tune_play_counter = 0;
        delay(100);
        break;

      case 4:
        // 4 clicks - show HELP
        HELP();
        break;

      case 5:
        // 5 clicks -  show info about sid file
        infoSID();
        break;

      case 6:
        // 6 clicks -
        // tralala
        break;

      case 7:
        // 7 clicks -  switch between random and alphabetical playlist
        // tralala
        break;
    }
    }
  */

  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 90_debug.ino
  //
  //////////////////////////////////////////////////////////////////////////////////////////////




  // set serial
  inline void debugInit () {

#ifdef USE_SERIAL
    Serial.begin(SERIAL_SPEED); // while (!Serial) {    yield();  }
#endif
#ifdef USE_SERIAL1
    Serial1.begin(SERIAL_SPEED); //while (!Serial1) {    yield();  }
#endif
    delay(2000);
  }
  //  Serial.print
  inline void debugPrintTXT (const char* output) {
#ifdef USE_SERIAL
    Serial.print(output);
#endif

#ifdef USE_SERIAL1
    Serial1.print(output);
#endif
  }

  //  Serial.println
  inline void debugPrintTXTln (const char* output) {
#ifdef USE_SERIAL
    Serial.println(output);
#endif
#ifdef USE_SERIAL1
    Serial1.println(output);
#endif
  }

  //  Serial.print
  inline void debugPrintNUMBER (const int32_t output) {
#ifdef USE_SERIAL
    Serial.print(output);
#endif

#ifdef USE_SERIAL1
    Serial1.print(output);
#endif
  }

  //  Serial.print(nn,HEX)
  inline void debugPrintNUMBERHEX (const int32_t output) {
#ifdef USE_SERIAL
    Serial.print("$");
    Serial.print(output, HEX);
#endif

#ifdef USE_SERIAL1
    Serial1.print("$");
    Serial1.print(output, HEX);
#endif
  }




  // name z_ so Arduino IDE will have this ino file loaded last, after all other definitions and voids


  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // setup
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  void setup() {



    // pinMode(PB13, OUTPUT);
    // pinMode(PB12, OUTPUT);
    randomSeed(millis());
 AllocateRAM();                     // in 02_ram.ino     // allocate available memory (max 65535), leave 2048 bytes for locals

    debugInit(); // in 90_debug.ino
    Loader(); // in 10_INTERNAL.ino
    CPU_test(); // benchmark 6502 emulator timing
    autoconfigMultiplier(); // benchmark SID emulator timing, set multiplier

    Tune = SID_current_tune;
    previous_Tune = Tune;


    infoSID(); // print out info on any output that is defined
    InitHardware(); // Setup timers and interrupts



    MySetup();


  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // main loop
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////


  void loop() {

    MyLoop();

  }
#endif



  // INFO:

  //      - only music based on raster irq (PSID V2+) (play address must not be 0, will not work with custom music player inside .sid file)
  //      - multi-speed .sid files are not supported, ( emulation of 6502 is not fast enough to keep up ).
  //      - .sid load address must be at $0400 or greater
  //      - total size of .sid file must be less then 8192 bytes
  //
  // STM32F103C8
  // This project is purely for my own entertainment , so WITHOUT ANY WARRANTY!
  // I had tried to make it as much as posible to be portable for other microcontrollers
  // My choise of STM32F103C8 is only because of it's price
  // in bluepill's RAM, player is at 0x300, .sid file data is at 0x400 (if microcontrollers RAM is less then end of memory address of sid file, otherwise file data is in it's original memory address)
  // tunes that work with it are in /tunes/f103/ subfolder


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
