

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


 Loader(); // in 10_INTERNAL.ino

    CPU_test(); // benchmark 6502 emulator timing
    autoconfigMultiplier(); // benchmark SID emulator timing, set multiplier
    InitHardware(); // Setup timers and interrupts
    FRAMEtest(); // test 1 frame
    HELP();

  


}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {








  ///////////////////////////////////////
  // Player loop
  ///////////////////////////////////////
 // go player


    exec6502(); // execute 1 instruction
    Emu_uS = Emu_uS + (ticktable[opcode]); // get emulated uS value


    // while (STAD4XX == 1) {}
    if (JSR1003 == 1) { // JSR1003 check
      JSR1003 = 0;

      checkButton1();

      // only time when JSR1003=1 and VIC_irq_request=1 at this point, is when emulated frame is not executed fast enough (Real_us>SID_speed) (tune will play slower, or "streched")

      if (VIC_irq_request == 0) {
        ///////////////////////////////////////
        //
        // insert code here to be executed once per frame, not longer then 7mS execution time
        //
        ///////////////////////////////////////

        /////////////////////////////////////////////////
        // button check (once per frame)
        // // 0=idle;   1,2,3,4... = short clicks;    -1 = is_pressed;

        digitalWrite(LED_BUILTIN, LOW); // the bright it is, the better. If it's mostly off, emulation is not fast enough for current tune speed


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
        // play next tune check (once per frame - tune timed out or button press)
        if (play_next_tune == true) { // changing subtune
          tune_play_counter = 0;

          if (SID_current_tune == SID_number_of_tunes) {
            SID_current_tune = 0;
          }

          SID_current_tune = SID_current_tune + 1;


          set_tune_speed ();
          infoSID();
          play_next_tune = false; // set speed and play next tune
          reset6502();
          reset_SID();
          POKE (0x0304, SID_current_tune - 1 ); // player's address for init tune

        } // play next tune check


      } // code per frame

      while (VIC_irq_request == 0) {
        // wait untill interrupt say it's new frame

      }
      digitalWrite(LED_BUILTIN, HIGH);

      VIC_irq_request = 0;
      instructions = 0;
      Emu_uS = 0;
      Real_uS_start = micros();


    } // JSR1003 check
  
}  // player loop   // main loop
