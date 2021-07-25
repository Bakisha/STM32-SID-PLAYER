
char SID_filename[128] ; // reserve 128 bytes for file name
char SID_DIR_name [128] ; // reserve 128 bytes for directory path name
char SID_path_filename [256] ; // reserve 256 bytes for full path of file
bool IS_SID_FILE = false; //  for extension check

int16_t current_folder = 0; // current playing folder from folderlist (0 indexed)
int16_t current_file = 0; // number of currently playing sid file in folder. Valid only when playable sid is found (1 indexed)

uint16_t total_sid_files = 0; // total number of sid files in directory

bool scan_directory = false;
bool count_sids = false;

bool mode_play_ON = true; // true=play, false=pause;


//  folder/file/tune  MODE      :
//                       0 - next (normal)
//                       1 - previous (only with buttons, then it will revert to normal (next)

int16_t folder_mode = 0;
int16_t file_mode = 0;
int16_t tune_mode = 0; //

bool change_folder = 0;
bool change_file = 0;
volatile bool change_tune = 0; // interrupt can change it, must be volatile
volatile bool player = false; // short version of ( change_folder | change_file | change_tune | load_sid ), interrupt can change it, must be volatile

bool load_sid = false; // only change_file can set it
bool sid_loaded = false;

SdFat sd;
SdFile folder;
SdFile sidfile;

uint8_t SDdata[10] ;

//////////////////////////////////////////////////////////////////////////////////////////

// delete settings file from SdCard
void DeleteSettings() {
  if (!sd.remove("STM32SID_Player_settings.bin") ) {
    debugPrintTXTln("settings remove failed");
  }
  else {
    debugPrintTXTln("settings file removed");
  }
}
//////////////////////////////////////////////////////////////////////////////////////////

// read settings file from SdCard ( period, multiplier, current_file, current_folder, total_sid_files in 16bit little endian format )
bool ReadSettings () {
  bool return_b = false;
  if (sd.exists("STM32SID_Player_settings.bin")) {

    debugPrintTXTln("settings file found");
    if (sidfile.open("STM32SID_Player_settings.bin", O_READ )) {

      debugPrintTXTln("settings file opened for reading");

      sidfile.seekSet( 0 );

      SDdata[0] = uint8_t (sidfile.read());
      SDdata[1] = uint8_t (sidfile.read());
      SDdata[2] = uint8_t (sidfile.read());
      SDdata[3] = uint8_t (sidfile.read());
      SDdata[4] = uint8_t (sidfile.read());
      SDdata[5] = uint8_t (sidfile.read());
      SDdata[6] = uint8_t (sidfile.read());
      SDdata[7] = uint8_t (sidfile.read());
      SDdata[8] = uint8_t (sidfile.read());
      SDdata[9] = uint8_t (sidfile.read());

      period          = uint32_t ( uint32_t(SDdata[0]) | uint32_t(SDdata[1] << 8));
      multiplier      = uint32_t ( uint32_t(SDdata[2]) | uint32_t(SDdata[3] << 8));
      current_file    = uint32_t ( uint32_t(SDdata[4]) | uint32_t(SDdata[5] << 8));
      current_folder  = uint32_t ( uint32_t(SDdata[6]) | uint32_t(SDdata[7] << 8));
      total_sid_files = uint32_t ( uint32_t(SDdata[8]) | uint32_t(SDdata[9] << 8));

      debugPrintTXTln("settings loaded");

      sidfile.close();

      return_b = true;

    }

  }

  return return_b;
}
//////////////////////////////////////////////////////////////////////////////////////////

// write settings file to root of SdCard ( period, multiplier, current_file, current_folder in 16bit little endian format )


void WriteSettings (uint32_t p, uint32_t m , uint32_t fl, uint32_t fd, uint32_t ts) {
  if (!sd.exists("STM32SID_Player_settings.bin")) {
    debugPrintTXTln("reading settings failed "); ("file don't exists");
    if (sidfile.open("STM32SID_Player_settings.bin", O_RDWR | O_CREAT)) { //  If O_CREAT and O_EXCL are set, open() shall fail if the file exists.


      debugPrintTXTln("file creating , please wait");
      sidfile.close();
      debugPrintTXTln("file created ");
    }
  }
  if (sidfile.open("STM32SID_Player_settings.bin", O_RDWR)) { //  If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
    //debugPrintTXTln("file is created and opened for writing");
    SDdata[0] = uint8_t (p & 0xff);
    SDdata[1] = uint8_t ((p & 0xff00) >> 8);
    SDdata[2] = uint8_t (m & 0xff);
    SDdata[3] = uint8_t ((m & 0xff00) >> 8);
    SDdata[4] = uint8_t (fl & 0xff);
    SDdata[5] = uint8_t ((fl & 0xff00) >> 8);
    SDdata[6] = uint8_t (fd & 0xff);
    SDdata[7] = uint8_t ((fd & 0xff00) >> 8);
    SDdata[8] = uint8_t (ts & 0xff);
    SDdata[9] = uint8_t ((ts & 0xff00) >> 8);
    sidfile.seekSet( 0 );
    sidfile.write(uint8_t(SDdata[0]));
    sidfile.write(uint8_t(SDdata[1]));
    sidfile.write(uint8_t(SDdata[2]));
    sidfile.write(uint8_t(SDdata[3]));
    sidfile.write(uint8_t(SDdata[4]));
    sidfile.write(uint8_t(SDdata[5]));
    sidfile.write(uint8_t(SDdata[6]));
    sidfile.write(uint8_t(SDdata[7]));
    sidfile.write(uint8_t(SDdata[8]));
    sidfile.write(uint8_t(SDdata[9]));
    //debugPrintTXTln("settings saved");
    sidfile.close();
  }
}


//////////////////////////////////////////////////////////////////////////////////////////

void initSD() {

  while (!sd.begin(CS_SDCARD, SD_SCK_MHZ(SD_SPEED))) { // 1Mhz - safest sd card speed
    debugPrintTXTln("ERROR1 - can't open sd card");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    // stop if can't open sd card
  }

  debugPrintTXTln("OK1 - SD started");

  //delay(1000);

}

/////////////////////////////////////////////////////////////////////////

void testROOT () {


  while (!folder.open("/")) {// stop if can't open root of sd card
    debugPrintTXTln("ERROR2 -  can't open root folder");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

  }

  debugPrintTXTln("OK2 - Root opened");

  folder.close();

  // delay(1000);

}
//////////////////////////////////////////////////////////////////////////////////////////


void LoadFirstSID() {

  current_folder = 0;
  current_file = 1;

  while (1) { // in a endless search for sids :-)

    total_sid_files = SD_Count_Total_SIDs(); // set total_sid_files


    debugPrintTXT ("Folder:  ");    debugPrintNUMBER (current_folder); debugPrintTXT ("   ");
    debugPrintTXT ("playable sid files "); debugPrintNUMBER (total_sid_files); debugPrintTXTln (" ");


    if (total_sid_files == 0) {// there is no sids in this folder, move along
      current_folder = current_folder + 1;
      current_file = 1;
      if (current_folder > (NUMBER_OF_FOLDERS - 1)) {
        current_folder = 0;
      }

    }
    else  {  // try to load it

      if (SD_LOAD()) { // founded and loaded

        change_tune = false;
        change_folder = false;
        change_file = false;
        player = true;
        load_sid = false;

        break; // abort while(1)
      }
      else { // failed load with this current_file value. continue searching
        current_file = current_file + 1;
        if (current_file > total_sid_files) {
          current_folder = current_folder + 1;
          current_file = 1;
          if (current_folder > (NUMBER_OF_FOLDERS - 1)) {
            current_folder = 0;
          }
        }
      } // failed load with this current_file value. continue searching
    } // try to load it
  } // while
} // LoadFirstSID



//////////////////////////////////////////////////////////////////////////////////////////




uint16_t SD_Count_Total_SIDs() { // set total_sid_files
  debugPrintTXT("Counting playable sids...");
  uint32_t number_of_sids = 0;

  // input: current_folder, current file

  // return: total_sid_files          - number of sid files in directory
  //

  // set name for current directory of FOLDER playlist
  strcpy (SID_DIR_name, "/");                                                  // empty string
  strcat (SID_DIR_name,  HVSC);                                               // add main HVSC path to string
  strcat (SID_DIR_name,  "/");
  strcat (SID_DIR_name,  FOLDER_PLAYLIST[ current_folder ]);                   // add directory name to string

  debugPrintTXT(".");

  if (folder.open(SID_DIR_name)) { // set folder

    PLAYABLE_SID = false; // assume it can't be played
    scan_directory = true;


    while (scan_directory == true) {

      if (sidfile.openNext(&folder, O_RDONLY)) { // keep opening next item in folder

        if (sidfile.isFile()) { // is it a file?

          if (sidfile.getName(SID_filename, 128) ) {};  // get name of the file/folder
          // debugPrintTXTln("");
          // debugPrintTXTln("--------------------------------------------------------");
          strcpy (SID_path_filename, "");                        // empty string
          strcat (SID_path_filename, SID_DIR_name);              // add directory name to string
          strcat (SID_path_filename, SID_filename);             // add filename to string,
          // full path of filename/folder

          if (strcmp(&SID_path_filename[strlen(SID_path_filename) - 4], ".sid") == 0) { // Is it sid file?

            SID_data_size = sidfile.fileSize();            // get size of a file


            //  load header to RAM at $0380 (so it can also be accessed by 6502 code)
            sidfile.seekSet( 0 );
            for (int header = 0; header < 0x7e; header++) {
              POKE (header + 0x0380, sidfile.read());
            }


            PLAYABLE_SID = Compatibility_check(); // set sid's globals (from RAM), true if sid is playble

            if (PLAYABLE_SID) {
              number_of_sids = number_of_sids + 1; // count only playable sids

            } // PLAYABLE_SID

          } // if ".sid"

        } // is it a file

      } // while   sidfile.openNext
      else {
        // end of folder
        scan_directory = false;
      }

      sidfile.close();
    } // while scan directory



  } // folder.open
  else {
    // can't open folder. Wrong name in folder playlist, or SD Card error. either way, return error. It will trigger loading next file, but since total_sid_files is 0, it will trigger loading next folder.
    // PLAYABLE_SID = false;

    // this shouldn't be. it is supposed to always be able to open folder
    // mybe wrong path in directory array list
    // maybe file pathname is longer then array. either way, just load next folder/file
    debugPrintTXTln("");
    debugPrintTXT("opening folder: ");
    debugPrintTXT(SID_DIR_name);
    debugPrintTXTln(" failed");
    debugPrintTXTln("- check your favorite folders list");
    debugPrintTXTln("- check your SD Card  connection");

  }

  folder.close();

  debugPrintNUMBER(number_of_sids); debugPrintTXTln("");

  return number_of_sids;

  // total_sid_files = number_of_sids; // set total_sid_files
}


/////////////////////////////////////////////////////////////////////////////////////////////


bool SD_LOAD () {

  //debugPrintTXTln("LOADING");

  int16_t number_of_sid_files = 0;


  // input: current_folder, current file

  // output: total_sid_files          - number of sid files in directory
  //
  // NOTE:   SID_filename is name of the last sid file in the folder when this function end. Don't use it for SID_info. TODO: Reserve 128 bytes to copy it to new array.



  // set name for current directory of FOLDER playlist
  strcpy (SID_DIR_name, "/");                                                  // empty string
  strcat (SID_DIR_name,  HVSC);                                               // add main HVSC path to string
  strcat (SID_DIR_name,  "/");
  strcat (SID_DIR_name,  FOLDER_PLAYLIST[ current_folder ]);                   // add directory name to string

  sid_loaded = false; // assume it will fail

  if (folder.open(SID_DIR_name)) { // set folder


    scan_directory = true;


    while (scan_directory == true) {

      if (sidfile.openNext(&folder, O_RDONLY)) { // keep opening next item in folder

        if (sidfile.isFile()) { // is it a file?

          if (sidfile.getName(SID_filename, 128) ) {};  // get name of the file/folder
          // debugPrintTXTln("");
          // debugPrintTXTln("--------------------------------------------------------");
          strcpy (SID_path_filename, "");                        // empty string
          strcat (SID_path_filename, SID_DIR_name);              // add directory name to string
          strcat (SID_path_filename, SID_filename);             // add filename to string,
          // full path of filename/folder

          if (strcmp(&SID_path_filename[strlen(SID_path_filename) - 4], ".sid") == 0) { // Is it sid file?

            SID_data_size = sidfile.fileSize();            // get size of a file

            //  load header to RAM at $0380 (so it can also be accessed by 6502 code)
            sidfile.seekSet( 0 );
            for (int header = 0; header < 0x7e; header++) {
              POKE (header + 0x0380, sidfile.read());
            }



            PLAYABLE_SID = Compatibility_check(); // set sid's globals (from RAM), true if sid is playble

            if (PLAYABLE_SID) {


              number_of_sid_files = number_of_sid_files + 1; // count only playable sids


              if (current_file == number_of_sid_files ) { // current file number match, load  and exit

                sid_loaded = true;
                scan_directory = false; // exit loader

                player_setup(); // copy player from ROM to RAM at 0x0300

                // SD_LOAD
                sidfile.seekSet(0x7e );
                for (uint32_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

                  if ( LOW_RAM == true) { // LOW_RAM from Compatibility_check
                    if (i <  uint16_t(RAM_SIZE - 0x0400) ) {
                      POKE (0x400 + i,  sidfile.read()) ; // load sid data to 0x400
                    }
                  }
                  if ( LOW_RAM == false)   {
                    if (i < (RAM_SIZE ) ) {
                      POKE (SID_load_start + i,  sidfile.read() ); // load sid data to original memory location
                    }
                  }
                }

                reset6502();


              }

            } // PLAYABLE_SID



          } // if ".sid"

        } // is it a file

      } // while   sidfile.openNext
      else {
        // end of folder
        scan_directory = false;
      }



      sidfile.close();
    } // while scan directory

    folder.close();

  } // folder.open
  else {
    // can't open folder. Wrong name in folder playlist, or SD Card error. either way, return error. It will trigger loading next file, but since total_sid_files is 0, it will trigger loading next folder.
    // PLAYABLE_SID = false;

    // this shouldn't be. it is supposed to always be able to open folder
    // mybe wrong path in directory array list
    // maybe file pathname is longer then array. either way, just load next folder/file
    debugPrintTXTln("");
    debugPrintTXT("opening folder: ");
    debugPrintTXT(SID_DIR_name);
    debugPrintTXTln(" failed");
    debugPrintTXTln("- check your favorite folders list");
    debugPrintTXTln("- check your SD Card  connection");



  }

  folder.close();
  //delay(1000);
  if (sid_loaded == true) {

    debugPrintTXTln("READY");
    return true;
  }
  else {
    debugPrintTXTln("LOAD ERROR");
    debugPrintTXTln("READY");
    return false;
  }

}
