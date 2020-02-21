

// name z_ so Arduino IDE will have this ino file loaded last, after all other definitions and voids


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// setup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  randomSeed(millis());
  debugInit(); // in 90_debug.ino

  autoconfigFinished = false; // do the autoconfig only once

  initSD();  // in 10_SDcard.ino

  play_next_folder = true; // change folder when entered loop
  load_next_file = true;  // load file when entered loop
  try_again = true;  // keep trying until playable sid file is found

}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {

  if (play_next_folder == true) {

    ChangeFolder();       // in 10_SDcard.ino

  } // folder changer



  if ( (play_next_folder == false) & (load_next_file == true) & (try_again == true) ) {

    fileLoader();        //in 10_SDcard.ino

  } // file loader


  // do autoconfig only one time when sid is loaded to RAM
  if (!autoconfigFinished) {
    if ( PLAYABLE_SID) {
      //debugPrintTXTln("");
      CPU_test(); // benchmark 6502 emulator timing
      autoconfigMultiplier(); // benchmark SID emulator timing, set multiplier
      InitHardware(); // Setup timers and interrupts
      FRAMEtest(); // test 1 frame
      //debugPrintTXTln("");
      HELP();
      autoconfigFinished = true; // do the autoconfig only once
    }
  }


  instructions = 0;
  Emu_uS = 0;
  VIC_irq_request = 0;
  if (JAMMED) {
    JAMMED = false;
    debugPrintTXTln ("JAMMED          - loading next file ");
    // add more line here depending on lcd's height
  }
  else {
    JAMMED = false;
    debugPrintTXTln ("");

  }
  ///////////////////////////////////////
  // Player loop
  ///////////////////////////////////////
  while ( (play_next_folder == false) & (load_next_file == false) & (try_again == false) ) { // go player


    if (instructions < uint32_t (500000)) { // detect sid init jam
      exec6502(); // execute 1 instruction
      Emu_uS = Emu_uS + (ticktable[opcode]); // get emulated uS value
    }
    else {
      // load next file when jammed
      JAMMED = true;
      //debugPrintTXTln ("JAMMED          - loading next file ");
      load_next_file = true;
      try_again = true;
      play_next_tune = false;

    }


    // while (STAD4XX == 1) {}
    if (JSR1003 == 1) { // JSR1003 check (or jam)
      JSR1003 = 0;
      if (VIC_irq_request == 0) {
        ///////////////////////////////////////
        //
        // insert code here to be executed once per frame, not longer then 7mS execution time
        //
        ///////////////////////////////////////

        /////////////////////////////////////////////////
        // button check (once per frame)
        // // 0=idle;   1,2,3,4... = short clicks;    -1 = is_pressed;

        digitalWrite(LED_BUILTIN, HIGH);

        checkButton1();

        /*
                // enable this for fun with Arduino IDE's serial plotter :-)
                Real_uS_end = micros();
                Real_uS = Real_uS_end - Real_uS_start;
                debugPrintTXT("(uS)  Real: ");
                debugPrintNUMBER(Real_uS );
                debugPrintTXT(" Emulated: ");
                debugPrintNUMBER(Emu_uS);
                debugPrintTXT(" Speed: ");
                debugPrintNUMBER(uint32_t((100 * Emu_uS) / Real_uS ));
                debugPrintTXT("%");
                debugPrintTXTln("");
                Emu_uS = 0;
                Real_uS = 0;
        */

        /////////////////////////////////////////////////
        // play next tune check (once per frame)
        if (play_next_tune == true) { // changing subutune
          tune_play_counter = 0;

          if (SID_current_tune == SID_number_of_tunes) {
            SID_current_tune = 1; // SID_current_tune - SID_number_of_tunes;
          }
          SID_current_tune = SID_current_tune + 1;

          if (SID_current_tune >= SID_default_tune) { // loop finished
            load_next_file = true;
            try_again = true;
            play_next_tune = false;
          }
          else {

            set_tune_speed ();
            infoSID();
            play_next_tune = false; // set speed and play next tune
            reset6502();
            reset_SID();
            POKE (0x0304, SID_current_tune - 1 ); // player's address for init tune
          }
        } // play next tune check


      } // code per frame

      while (VIC_irq_request == 0) {
        // wait untill interrupt say it's new frame

      }
      digitalWrite(LED_BUILTIN, LOW);

      VIC_irq_request = 0;
      instructions = 0;
      Emu_uS = 0;
      Real_uS_start = micros();


    } // JSR1003 check
  }   // player loop
}     // main loop
