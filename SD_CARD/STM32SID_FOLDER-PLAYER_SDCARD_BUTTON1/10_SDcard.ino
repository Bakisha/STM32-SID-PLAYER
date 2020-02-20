#include <SPI.h>
#include <SdFat.h>  // install from board manager or from:  https://github.com/greiman/SdFat

#define SD_CLK          PA5
#define SD_MISO         PA6
#define SD_MOSI         PA7

SdFat sd;
SdFile root;
SdFile sidfile;
SdFile nextfile;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void initSD () { // load header to RAM at $0380
  while (!sd.begin(CS_SDCARD, SD_SCK_MHZ(SD_SPEED))) { // 1Mhz - safest sd card speed
    debugPrintTXTln("fatal error - can't open sd card");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    // stop if can't open sd card
  }


  while (!root.open("/")) {// stop if can't open root of sd card
    debugPrintTXTln("fatal error -  can't open root folder");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

  }
  // open root for 2 reasons:
  // 1 - program stops if root folder can't be opened
  // 2 - it is closed first thing in loop function
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void header_SD () { // load header to RAM at $0380

  sidfile.seekSet( 0 );
  for (int header = 0; header < 0x7e; header++) {
    POKE (header + 0x0380, sidfile.read());
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void SD_LOAD() {

  sidfile.seekSet(0x7e );
  for (uint32_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

    if ( LOW_RAM == true) {
      if (i < (RAM_SIZE - 0x0400) ) {
        POKE (0x400 + i,  sidfile.read()) ;
      }
    }
    if ( LOW_RAM == false)   {
      if (i < (RAM_SIZE ) ) {
        POKE (SID_load_start + i,  sidfile.read() );
      }
    }
  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void ChangeFolder() {
  HELP ();
  debugPrintTXTln("");
  debugPrintTXTln("-------------------------------");
  debugPrintTXTln("    changing directory         ");
  debugPrintTXTln("-------------------------------");

  if (!favorites_finished) {


    if (RANDOM_FOLDERS) {
      current_folder = random(NUMBER_OF_FAVORITE_FOLDERS); // as much "random" it is
    }
    else { // not random folder
      if (current_folder >= NUMBER_OF_FAVORITE_FOLDERS) {
        current_folder = 0;

#ifdef NUMBER_OF_ALL_FOLDERS // if HVSC folders is included
        favorites_finished = !favorites_finished;
#endif
      }
      else {
        current_folder = current_folder + 1;
      }
    }

    // get name for current directory
    strcpy (SID_DIR_name, "");                                                  // empty string
    strcat (SID_DIR_name,  HVSC);                                               // add main HVSC path to string
    strcat (SID_DIR_name,  "/");
    strcat (SID_DIR_name,  HVSC_FAVORITES[ current_folder ]);                   // add directory name to string

  }
  else {
#ifdef NUMBER_OF_ALL_FOLDERS // if HVSC folders is included

    if (RANDOM_FOLDERS) {
      current_folder = random(NUMBER_OF_ALL_FOLDERS); // as much "random" it is
    }
    else { // not random folder
      if (current_folder >= NUMBER_OF_ALL_FOLDERS) {
        current_folder = 0;

        favorites_finished = !favorites_finished;

      }
      else {
        current_folder = current_folder + 1;
      }
    }
    strcpy (SID_DIR_name, "");                                                  // empty string
    strcat (SID_DIR_name,  HVSC);                                               // add main HVSC path to string
    strcat (SID_DIR_name,  "/");
    strcat (SID_DIR_name,  HVSC_ALL[ current_folder ]);                         // add directory name to string
#else
    favorites_finished = !favorites_finished; //break;
#endif

  }



  root.close(); // close root, before setting new directory

  if (root.open(SID_DIR_name)) { // set directory, keep root opened until new directory is needed

    play_next_folder = false;
    load_next_file = true;
    try_again = true;


  }
  else {
    // this shouldn't be. it is supposed to always be able to open folder
    // mybe wrong path in directory array list
    // maybe file pathname is longer then array. either way, just load next folder/file
    debugPrintTXTln("");
    debugPrintTXT("opening folder: ");
    debugPrintTXT(SID_DIR_name);
    debugPrintTXTln(" failed");
    debugPrintTXTln("- check your favorite folders list");
    debugPrintTXTln("- check your SD Card  connection");

    play_next_folder = true;
    load_next_file = true;
    try_again = true;
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////////

inline void fileLoader() {


  if (nextfile.openNext(&root, O_RDONLY)) { // keep opening next file

    if (nextfile.getName(SID_filename, 256) ) {};  // get name of the file/folder
    debugPrintTXTln("");
    // debugPrintTXTln("--------------------------------------------------------");
    strcpy (SID_path_filename, "");                        // empty string
    strcat (SID_path_filename, SID_DIR_name);              // add directory name to string
    strcat (SID_path_filename, SID_filename);             // add filename to string,
    // full path of filename/folder





    if (nextfile.isFile()) { // is it a file?

      if (strcmp(&SID_path_filename[strlen(SID_path_filename) - 4], ".sid") == 0) { // Is it sid file?
        IS_SID_FILE = true;

        debugPrintTXTln("-----------------S-I-D----------------------------------");
        debugPrintTXTln(SID_path_filename);
        debugPrintTXTln("--------------------------------------------------------");

        SID_data_size = nextfile.fileSize();            // get size of a file

        // now load sid to memory and do compatibility check

        // reset values , in case opening sid  file fail

        RAM_OVERFLOW = true;
        VERSION = 0;
        MagicID = 0;
        PLAYABLE_SID = false; // assume it can't be played

        if ( sidfile.open( SID_path_filename, O_RDONLY ) ) {  // open file for reading

          header_SD(); //  load header to RAM at $0380 (so it can also be accessed by 6502 code)
          PLAYABLE_SID = Compatibility_check(); // set sid's globals (from RAM), true if sid is playble

          if (PLAYABLE_SID) {
            player_setup();
            SD_LOAD();
            reset6502();
            //infoSID(); // print out info on any output that is defined
          }
          sidfile.close();

        }
        else { // wtf? error opening sid file
          debugPrintTXTln("");
          debugPrintTXTln("fatal SD Card error:");
          debugPrintTXTln("error opening sid file");
          // load next
          play_next_folder = false;
          load_next_file = true;
          try_again = true;

        }

        if ( PLAYABLE_SID) { // play tune if no errors
          infoSID(); // print out info on any output that is defined
          reset6502(); // reset CPU
          POKE (0x0304, SID_current_tune - 1) ; // set tune number

          play_next_folder = false;
          load_next_file = false;
          try_again = false;
        }
        else {
          infoSID(); // print out info on any output that is defined

          play_next_folder = false;
          load_next_file = true;
          try_again = true;
        }
      }
      else {
        IS_SID_FILE = false;
        play_next_folder = false;
        load_next_file = true;
        try_again = true;

        debugPrintTXTln("----------N-O-T---S-I-D---F-I-L-E-----------------------");
        debugPrintTXTln(SID_path_filename);
        debugPrintTXTln("--------------------------------------------------------");
      }

    }
    else { // it's directory, get ready for next loop

      play_next_folder = false;
      load_next_file = true;
      try_again = true;
      debugPrintTXTln("---------------D-I-R-E-C-T-O-R-Y------------------------");
      debugPrintTXTln(SID_path_filename);
      debugPrintTXTln("--------------------------------------------------------");


      play_next_folder = false;
      load_next_file = true;
      try_again = true;
    }

    nextfile.close(); // close it, so it will be ready for next one
  }
  else { // end of directory, time to change directory
    debugPrintTXTln("");
    debugPrintTXTln("-------------------------------");
    debugPrintTXTln("    changing directory         ");
    debugPrintTXTln("-------------------------------");
    play_next_folder = true;
    load_next_file = true;
    try_again = true;
  }


}
