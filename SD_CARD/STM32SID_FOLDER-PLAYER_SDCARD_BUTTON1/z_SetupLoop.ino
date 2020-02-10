

// name z_ so Arduino IDE will have this ino file loaded last, after all other definition


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// setup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  debugInit();

  autoconfigFinished = false; // do the autoconfig only once

  while (!sd.begin(CS_SDCARD, SD_SCK_MHZ(1))) { // 1Mhz - safest sd card speed
    debugPrintTXTln("fatal error - can't open sd card");
    while (1) {} // stop if can't open sd card
  }


  if (root.open("/")) {
    // open root for 2 reasons:
    // 1 - program stops if root folder can't be opened
    // 2 - it closed first thing in loop function
    play_next_folder = true; // change folder when entered loop
    load_next_file = true;  // load file when entered loop
    try_again = true;
  }
  else {
    debugPrintTXTln("fatal error -  can't open root folder");
    while (1) {} // stop if can't open root of sd card
  }
  LOGO();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {

  if (play_next_folder == true) {
    // get name for current directory
    strcpy (SID_DIR_name, "");                                                  // empty string
    strcat (SID_DIR_name,  HVSC_DIR[random( NUMBER_OF_FOLDERS )]);           // add directory name to string
    //debugPrintTXTln("DIR: ");
    //debugPrintTXTln(SID_DIR_name);

    root.close(); // close root, before setting new directory
    if (root.open(SID_DIR_name)) { // set directory, keep root opened until new directory is needed

      play_next_folder = false;
      load_next_file = true;
      try_again = true;

      // debugPrintTXTln(" ");
      // debugPrintTXTln("folder opened");
    }
    else {
      // this shouldn't be. it is supposed to always be able to open folder
      debugPrintTXTln(" ");
      debugPrintTXT("opening folder: ");
      debugPrintTXT(SID_DIR_name);
      debugPrintTXTln(" failed");
      debugPrintTXTln("check your folder list");
      debugPrintTXTln("check your SD Card  connection");
      // while (1) {} // stop if can't open folder

      play_next_folder = true;
      load_next_file = true;
      try_again = true;
    }
  }



  if ( (play_next_folder == false) & (load_next_file == true) & (try_again == true) ) {


    if (nextfile.openNext(&root, O_RDONLY)) { // keep opening next file

      if (nextfile.getName(SID_filename, 256) ) {};  // get name of the file/folder
      debugPrintTXTln("");
      debugPrintTXTln("--------------------------------------------------------");
      strcpy (SID_path_filename, "");                        // empty string
      strcat (SID_path_filename, SID_DIR_name);              // add directory name to string
      strcat (SID_path_filename, SID_filename);             // add filename to string,
      // full path of filename/folder

      debugPrintTXTln(SID_path_filename);
      debugPrintTXTln("--------------------------------------------------------");


      if (nextfile.isFile()) { // is it a file?


        // TODO .sid extension check

        SID_data_size = nextfile.fileSize();            // get size of a file

        // now load sid to memory and do compatibility check

        // reset values , in case opening sid  file fail, so it won't play tune based on previous tune info

        RAM_OVERFLOW = true;
        VERSION = 0;
        MagicID = 0;

        if ( sidfile.open( SID_path_filename, O_RDONLY ) ) {  // open file for reading

          header_SD();
          Compatibility_check();
          player_setup();
          SD_LOAD();

          infoSID();
          reset6502();
          sidfile.close();

        }
        else { // wtf? error opening sid file
          debugPrintTXTln(" ");
          debugPrintTXTln("fatal SD Card error:");
          debugPrintTXTln("error opening sid file");
          // load next
          play_next_folder = false;
          load_next_file = true;
          try_again = true;
        }

        if ( (MagicID == 0x50) & (!RAM_OVERFLOW) & (VERSION > 1) ) { // play tune if no errors

          reset6502(); // reset CPU
          RAM[0x0304] = SID_current_tune - 1 ; // set tune number
          debugPrintTXT("OK");
          play_next_folder = false;
          load_next_file = false;
          try_again = false;
        }
        else {
          debugPrintTXT("ERROR");
          play_next_folder = false;
          load_next_file = true;
          try_again = true;
        }


      }
      else { // it's directory, get ready for next loop
        //debugPrintTXTln("it's a directory, skipping");
        play_next_folder = false;
        load_next_file = true;
        try_again = true;
      }
      nextfile.close(); // close it, so it will be ready for next one
    }
    else { // end of directory, time to change directory

      debugPrintTXTln("-------------------------------");
      debugPrintTXTln("    changing directory         ");
      debugPrintTXTln("-------------------------------");
      play_next_folder = true;
      load_next_file = true;
      try_again = true;
    }

  } // file loader




  ///////////////////////////////////////
  // Player loop
  ///////////////////////////////////////
  while ( (play_next_folder == false) & (load_next_file == false) & (try_again == false) ) { // go player


    if (!autoconfigFinished) { // this should be in setup, but need to load sid file to RAM from sd card before that
      debugPrintTXTln(" ");
      CPU_test(); // benchmark 6502 emulator timing
      autoconfigMultiplier(); // benchmark SID emulator timing, set multiplier
      InitHardware(); // Setup timers and interrupts
      FRAMEtest(); // test 1 frame and report to Serial
      debugPrintTXTln(" ");
      autoconfigFinished = true; // do the autoconfig only once
    }



    if (play_next_tune == true) { // changing subutune
      play_next_tune = false;
      reset6502();
      reset_SID();
      RAM[0x0304] = SID_current_tune - 1 ;
    }


    exec6502(); // execute 1 instruction
    Emu_uS = Emu_uS + (ticktable[opcode]); // get emulated uS value


    // while (STAD4XX == 1) {}
    if (JSR1003 == 1) { // JSR1003 check
      JSR1003 = 0;
      if (VIC_irq_request == 0) { // do this code once per frame
        ///////////////////////////////////////
        //
        // insert code here, not longer then 7mS execution time
        //
        ///////////////////////////////////////

        if ( (digitalRead(BUTTON_1) == LOW) ) { // button check // TODO: add short and long button press
          tune_play_counter = tune_play_next; // easiest way to end playing tune
          debugPrintTXT(".");
          delay(100);
        }

        /*
          // enable this for fun with Arduino IDE's serial plotter :-)
          Real_uS_end = micros();
          Real_uS = Real_uS_end - Real_uS_start;
          debugPrintTXT("Real: ");
          debugPrintNUMBER(Real_uS );
          debugPrintTXT(" Emulated: ");
          debugPrintNUMBER(Emu_uS);
          debugPrintTXTln(" ");
          Emu_uS = 0;
          Real_uS = 0;
        */
      }
      while (VIC_irq_request == 0) {
        // wait untill irq say it's new frame
      }
      VIC_irq_request = 0;
      instructions = 0;
      Emu_uS = 0;
      Real_uS_start = micros();
    }
  } // player loop


}
