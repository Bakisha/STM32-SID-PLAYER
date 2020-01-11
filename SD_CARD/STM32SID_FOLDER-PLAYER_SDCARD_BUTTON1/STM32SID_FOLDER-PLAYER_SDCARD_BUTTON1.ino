// it's recommended to set optimatization on FASTEST -O3 (from tool menu of Arduino IDE). 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Change these values to match your board/microcontroller (currently set for bluepill (STM32F103C8 - any core )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RAM_SIZE 0x3400
uint8_t DEFAULT_SONG = 1;
const uint8_t period = 4;
const uint8_t multiplier = 48 ; 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Change these values to set player configuration
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set directory path with sid files in it (  must end with "/" )
// char * SID_DIR_start = "/"; // root of sd card

char * SID_DIR_start = "/HVSC/MUSICIANS/B/Blues_Muz/";  
#define TUNE_PLAY_TIME 180 // in seconds

#define USE_FILTERS

#define FILTER_FREQUENCY 8000
#define FILTER_SENSITIVITY  4

#define USE_CHANNEL_1
#define USE_CHANNEL_2
#define USE_CHANNEL_3

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hardware
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_OUT PA8 // can't be changed, this is just reminder

#define BUTTON_1 PB0 // can work without button, it will skip to next tune when timed out

#define CS_SDCARD       PA1 // can be changed
#define SD_CLK          PA5
#define SD_MISO         PA6
#define SD_MOSI         PA7

#include <SPI.h>
#include <SdFat.h>

SdFat sd;
SdFile root;
SdFile sidfile;
SdFile nextfile;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end of settings
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef USE_HAL_DRIVER
#define USE_STM32_ST_CORE
#endif

#ifdef __STM32F1__
#define USE_STM32duino_CORE
#endif

#include "SID.h"
#include "6502.h"

void setup() {

  InitHardware();

  while (!sd.begin(CS_SDCARD, SD_SCK_MHZ(1))) { // 1Mhz - safest sd card speed
    error_sound_SD();
  }

  strcpy(SID_DIR_name, SID_DIR_start);

  while (!root.open(SID_DIR_name)) {
    error_sound_ROOT();
  }

}

void loop() {

  load_next_file = false;
  try_again = true;
  while (try_again) {
    if (nextfile.openNext(&root, O_RDONLY)) {
      if (nextfile.isFile()) {
        // TODO .sid extension check
        if (nextfile.getName(SID_file_name, 256) ) {};
        SID_data_size = nextfile.fileSize();
        try_again = false;
      }
      else {
        error_open_file();
      }
    }
    else {
      root.close();
      while (!root.open(SID_DIR_name)) {
        error_open_folder () ;
      }
    }
    nextfile.close();
  }

  strcpy (SID_filename, "");
  strcat (SID_filename, SID_DIR_name) ;
  strcat (SID_filename, SID_file_name) ;



  if ( !sidfile.open( SID_filename, O_RDONLY ) ) {
    error_open_sid();
  }
  else {

    header_SD();
    Compatibility_check();
    player_setup();
    SD_LOAD();
    reset6502();
    sidfile.close();
  }

  if ( (MagicID == 0x50) & (!RAM_OVERFLOW) & (VERSION > 1) ) {
    play_next_tune = false;
    load_next_file = false;
    reset6502();
    RAM[0x0304] = SID_current_tune - 1 ;
    while (!load_next_file) {
      if (play_next_tune == true) {

        play_next_tune = false;
        reset6502();
        RAM[0x0304] = SID_current_tune - 1 ;
      }
      PB12_HIGH;
      exec6502();
      PB12_LOW ;
      if ( (digitalRead(BUTTON_1) == LOW) ) {
        tune_play_counter = tune_play_next;
        delay(500);
      }
      // while (STAD4XX == 1) {}
      if (JSR1003 == 1) { // JSR1003 check
        JSR1003 = 0;
        if (VIC_irq_request == 0) {
        }
        while (VIC_irq_request == 0) { // wait untill irq say it's new frame
        }
        VIC_irq_request = 0;
        instructions = 0;
      }
    }
  }
  else {
    error_PSID_V2_RAM_OVERFLOW();
  }
}
