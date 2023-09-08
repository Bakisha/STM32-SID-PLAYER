

// name z_ so Arduino IDE will have this ino file loaded last, after all other definitions and voids


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// setup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:

  AllocateRAM();                     // in 02_ram.ino     // allocate available memory (max 65535), leave 2048 bytes for locals
  debugInit ();                      // in 90_debug.ino

  initSD ();                         // in 10_SD.ino
  testROOT ();                       // in 10_SD.ino

#ifdef BUTTON_NEXT                                // delete settings file if NEXT button is pressed on power up
  pinMode(BUTTON_NEXT, INPUT_PULLUP);             // this will be set in 20_hardware.ino anyway
  if (digitalRead(BUTTON_NEXT) == LOW) {
    DeleteSettings();                             // in 10_SD.ino
  }
#endif

  // DeleteSettings(); // testing or lock-up






  tune_mode       = 0;                          // tune mode   : 0-> next ; 1-> previous
  file_mode       = 0;                          // file mode   : 0-> next ; 1-> previous
  folder_mode     = 0;                          // folder mode : 0-> next ; 1-> previous
  change_tune     = false;                      //
  change_file     = false;                      // file/folder number loaded from SdCard
  change_folder   = false;                      //
  count_sids      = false;                      //
  load_sid        = false;                      // load current_file/current_folder tune
  mode_play_ON    = true;                       // enable it in case it was off (for pause ON/OFF)
  player          = false;                      // true: play sids, check buttons  , false: check change_tune/file/folder/load_sid


  // read period/multiplier/current_file/current_folder from SdCard
  if (ReadSettings()) {                           // in 10_SD.ino
    debugPrintTXTln("reading settings OK ");

    if (SD_LOAD()) {                                    // load sid file
      // everything else was set in SD_LOAD
      player = true;
      infoSID(); // print out info on any output that is defined
      instructions = 0; // reset JAM counter when finished loading
      tune_play_counter = 0; // reset tune counter
      mode_play_ON    = true;                       // enable it in case it was off
    }
    else { // fail to load file from settings, load first sid, do the autonconfig, autonconfig multiplier

      debugPrintTXTln("reading settings failed ");
      LoadFirstSID ();                   // in 10_SD.ino
      CPU_test();                        // benchmark 6502 emulator timing
      autoconfigMultiplier();            // benchmark SID emulator timing, set multiplier
    }
  }
  else                                            // no settings file loaded, do the autoconfig
  { //                                               NOTE: first creation of a settings file might take some time, around 15 seconds
    debugPrintTXTln("reading settings failed ");

    LoadFirstSID ();                   // in 10_SD.ino
    CPU_test();                        // benchmark 6502 emulator timing
    autoconfigMultiplier();            // benchmark SID emulator timing, set multiplier

    WriteSettings (period, multiplier , current_file, current_folder, total_sid_files);     // in 10_SD.ino
  }

  // multiplier = 24; // override autoconfig (samplerate = 1.000.000,00 / multiplier )
  // period = 4; // pwm width in ?S, override autonconfig
  debugPrintTXT(" period: ");  debugPrintNUMBER(period);  debugPrintTXT(" multiplier: ");  debugPrintNUMBER(multiplier);  debugPrintTXT(" current_file: ");  debugPrintNUMBER(current_file);  debugPrintTXT(" current_folder: ");  debugPrintNUMBER(current_folder);  debugPrintTXT(" total_sid_files: ");  debugPrintNUMBER(total_sid_files);  debugPrintTXTln(" ");

  InitHardware();                    // Setup hardware timers and interrupts
 
  reset_SID();
  reset6502();
  HELP();
  infoSID();                         // print out info on any output that is defined



  /*
    // manual set folder number
    // current_file=0;                            // so it will load 1st file // no need, change_folder will set it to 1st
    current_folder = random(NUMBER_OF_FOLDERS);   // why first random is always the same?
    // current_folder = NUMBER_OF_FOLDERS;        // so it will load 1st folder in the list

    tune_mode = 0;                                // tune mode   : 0-> next ; 1-> previous
    file_mode = 0;                                // file mode   : 0-> next ; 1-> previous
    folder_mode = 0;                              // folder mode : 0-> next ; 1-> previous
    change_tune = false;
    change_file = false;
    change_folder = true;
    count_sids = true;                            // if just switched from loading from folder then count_sids = true
    mode_play_ON = true; // enable it in case it was off
    player = false; // true: play sids, check buttons  , false: check change_tune/file/folder/load_sid
  */

  tune_play_counter = 0;                          // reset tune counter
  instructions = 0;                               // reset JAM

}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
  // put your main code here, to run repeatedly:

  if ( player == true ) { // go player

    ///////////////////////////////////////
    // Player loop
    ///////////////////////////////////////

    if (instructions < uint32_t (500000)) { // detect sid init jam, or player's overload
      if (mode_play_ON) {
        exec6502(); // execute 1 instruction
        //Emu_uS = Emu_uS + (ticktable[opcode]); // get emulated uS value
      }
      else {
        MASTER_VOLUME = 0; // pssst
        JSR1003 = 1; // enabled because of buttons read
      }
    }
    else {
      // load next file when jammed
      debugPrintTXTln ("JAMMED          - loading next file ");

      change_file = true;
      player = false;
    }

    if (JSR1003 == 1) { // JSR1003 check
      JSR1003 = 0;


      /////////////////////////////////////////////////
      // button check (once per frame)
      // // 0=idle;   1,2,3,4... = short clicks;    -1 = is_pressed;

      checkButton_PREV();
      checkButton_PLAY();
      checkButton_NEXT();



      /////////////////////////////////////////////////
      // IR check (once per frame)
      //
     
      checkIR ();  // in 15_IR.ino


      // only time when (JSR1003=1 and VIC_irq_request=1) at this point, is when emulated frame is not executed fast enough (Real_us>SID_speed) (tune will play slower, or "streched"). It will trigger JAM if it's constantly on

      if (VIC_irq_request == 0) {
        ///////////////////////////////////////
        //
        // insert code here to be executed once per frame, not longer then few miliseconds execution time
        //
        ///////////////////////////////////////


#ifdef LED_BUILTIN
        digitalWrite(LED_BUILTIN, LOW); // Overload status , emulation is not fast enough for current tune speed
#endif



        /////////////////////////////////////////////////


        instructions = 0; // reset instruction. Rather, don't reset instruction number if it's constantly overloaded, so it will trigger JAM

      } // code per frame

      while (VIC_irq_request == 0) {        // wait untill virtual interrupt say it's new frame


      }
#ifdef LED_BUILTIN
      digitalWrite(LED_BUILTIN, HIGH);
#endif


      VIC_irq_request = 0; // reset virtual irq

    } // JSR1003 check

  } // player true

  else  { //  "player" is disabled

    reset_SID(); // mute output while searching and loading sid


    ///////////////////////////////////////
    // Loader
    ///////////////////////////////////////

    // only one of tune/file/folder/load_sid is enabled at a time
    // only "change_folder" can activate "count_sids"
    // only "change_file" can activate "load_sid"
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (change_tune) {

      change_tune = false;

      switch (tune_mode) {
        case 0: // next

          tune_play_counter = 0; // reset tune time counter (hopefully loading will not be longer then 3 minutes)
          SID_current_tune = SID_current_tune + 1;
          if (SID_current_tune > SID_number_of_tunes) {
            SID_current_tune = 1;
          }
          if (SID_current_tune == SID_default_tune) { // tune loop finished
            change_file = true;
          }
          else {
            reset_SID();
            set_tune_speed ();
            infoSID();
            reset6502();
            POKE (0x0304, SID_current_tune - 1 ); // player's address for init tune
            player = true;            // set speed and play next tune
          }
          break;
        case 1: // previous
          tune_play_counter = 0; // reset tune time counter (hopefully loading will not be longer then 3 minutes)
          SID_current_tune = SID_current_tune - 1;
          if (SID_current_tune == 0 ) {
            SID_current_tune = SID_number_of_tunes;
          }
          tune_mode = 0; // back to next
          reset_SID();
          set_tune_speed ();
          infoSID();
          reset6502();
          POKE (0x0304, SID_current_tune - 1 ); // player's address for init tune
          player = true;            // set speed and play next tune
          break;
      } // switch tune_mode

    } // if change_tune

    ///////////////////////////////////////////////////////////////////////////////////////////

    if (change_file) {

      change_file = false;

      switch (file_mode) {
        case 0: // next
          //
          if (count_sids) { // count files only on changed folder
            count_sids = false;
            total_sid_files =  SD_Count_Total_SIDs(); // set total_sid_files
          }
          current_file = current_file + 1; // 1 indexed

          if (current_file > total_sid_files) { // last file played, load next folder
            folder_mode = 0; // next folder
            change_folder = true;
          }
          else { // load next file
            load_sid = true;
          }
          break;

        case 1: // previous
          file_mode = 0 ; // reset file mode
          current_file = current_file - 1; // 1 indexed
          if (current_file < 1) { // play again 1st file
            current_file = 1; // 1 indexed
            load_sid = true; // stay on first file

          }
          else { // play previous file
            load_sid = true;
          }
          break;
      } // switch file_mode

    } // if change_file

    ///////////////////////////////////////////////////////////////////////////////////////////
    if (change_folder) {

      change_folder = false;

      switch (folder_mode) {
        case 0: // next
          current_folder = current_folder + 1;
          if (current_folder > (NUMBER_OF_FOLDERS - 1)) {
            current_folder = 0; // zero indexed
          }
          current_file = 0; // load 1st file
          change_file = true;  count_sids = true;
          break;
        case 1: // previous folder
          if (current_folder == 0) { // play last folder if 0
            current_folder = (NUMBER_OF_FOLDERS - 1); // zero indexed
          }
          else {
            current_folder = current_folder - 1;
          }
          folder_mode = 0;  // reset folder mode
          current_file = 0; // load 1st file
          change_file = true; count_sids = true;
          break;
      } // switch folder_mode

    } // if change_folder

    ///////////////////////////////////////////////////////////////////////////////////////////
    if (load_sid == true) {
      debugPrintTXTln ("LOADING...");

      load_sid = false;
      if ((total_sid_files) > 0) {

        if (SD_LOAD()) {
          // everything else was set in SD_LOAD
          CIA_DC05 = 0;
          CIA_DC04 = 0;
          player = true;
          infoSID(); // print out info on any output that is defined
          instructions = 0; // reset JAM counter when finished loading
          tune_play_counter = 0; // reset tune counter
          WriteSettings (period, multiplier , current_file, current_folder, total_sid_files);     // in 10_SD.ino
        }
        else { // idiot-proof else. Just load next file, if any
          change_file = true;
        }

      } // > 0
      else { // 0 playable sid files in this directory
        folder_mode = 0; // next folder
        change_folder = true; // next folder
      } // = 0
    } // load_sid
  } // else player
}     // main loop
