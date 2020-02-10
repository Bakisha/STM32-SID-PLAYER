

// name z_ so Arduino IDE will have this ino file loaded last, after all other definition


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// setup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  debugInit();

  autoconfigFinished = false; // do the autoconfig only once

  LOGO();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {


  RAM_OVERFLOW = true;
  VERSION = 0;
  MagicID = 0;

  header_INTERNAL();
  Compatibility_check();
  player_setup();
  INTERNAL_LOAD();
  infoSID();
  reset6502();


  if ( (MagicID == 0x50) & /* (!RAM_OVERFLOW) & */ (VERSION > 1) ) { // play tune if no errors (RAM_OVERFLOW allowed, so at least reading is possible

    reset6502(); // reset CPU
    RAM[0x0304] = SID_current_tune - 1 ; // set tune number
    debugPrintTXT("OK");

  }
  else {
    debugPrintTXT("ERROR");
    //while {1};
  }



  ///////////////////////////////////////
  // Player loop
  ///////////////////////////////////////
  while ( 1 ) { // go player


    if (!autoconfigFinished) { // this should be in setup, but need to load sid file to RAM from sd card before that
      debugPrintTXTln(" ");
      CPU_test(); // benchmark 6502 emulator timing
      autoconfigMultiplier(); // benchmark SID emulator timing, set multiplier
      InitHardware(); // Setup timers and interrupts
      FRAMEtest(); // test 1 frame and report to Serial
      debugPrintTXTln(" ");
      autoconfigFinished = true; // do the autoconfig only once
    }



    if (play_next_tune == true) { // changing subtune
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
