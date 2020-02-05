// name z_ so Arduino IDE will have this ino file loaded last, after all other definition

uint32_t Real_uS_start;
uint32_t Real_uS_end;
uint32_t Emu_uS = 0;
uint32_t Real_uS = 0 ;
uint32_t CPU_uS = 0;
uint32_t SID_uS = 0;
uint32_t best_multiplier = 1;
uint32_t estimated_frame_time = 0;
uint32_t test_instructions = 1000; // number of instructions // around 4 minutes of sid play
///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// setup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  debugInit();

  debugPrintTXTln(" ");
  debugPrintTXTln("Test started");

  // calculate how well 6502 can be emulated

  Init6502(); // Emulator/player initialization // load sid to RAM

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



  debugPrintTXT   ("Microcontroller speed:               ");
  debugPrintNUMBER(F_CPU / 1000000);
  debugPrintTXTln (" MHz");


  debugPrintTXTln (" ");
  debugPrintTXTln ("6502 emulation only:");
  debugPrintTXT    ("Instructions executed:               ");
  debugPrintNUMBER(instructions);
  debugPrintTXTln  (" ");
  debugPrintTXT   ("Real time passed:                    ");
  debugPrintNUMBER(Real_uS);
  debugPrintTXTln (" uS");
  debugPrintTXT   ("Emulated time passed:                ");
  debugPrintNUMBER(Emu_uS);
  debugPrintTXTln (" uS");
  debugPrintTXT   ("Emulated CPU speed:                  ");
  debugPrintNUMBER(uint32_t((100 * Emu_uS) / Real_uS ));
  debugPrintTXTln ("%");
  debugPrintTXT   ("Average time per CPU instruction:    ");
  debugPrintNUMBER(CPU_uS );
  debugPrintTXTln (" uS");


  // calculate how long IRQ will last based on chosen microcontroller

  Emu_uS = 0;
  Real_uS = 0;


  multiplier = 1;
  best_multiplier = 1;

  while (best_multiplier == 1) { //

    Real_uS_start = micros();
    for (uint32_t total_instructions = 0; total_instructions < test_instructions; total_instructions++) {
      SID_Emulator();
    }
    Real_uS_end = micros();
    Real_uS = Real_uS_end - Real_uS_start;
    SID_uS = Real_uS / test_instructions;
    estimated_frame_time = ((20000 / multiplier) * (SID_uS + 2)) + (500 * (CPU_uS )); // estimated around 500 instruction per frame (20mS) should be enough for calculations
    if (estimated_frame_time < 13000) { // worst case: 13000 for bluepill at 48MHz, O0 (smallest code) optimatization
      best_multiplier = multiplier ;
    }
    /*
        debugPrintTXTln("Testing SID Emulator");
        debugPrintTXT("SID emulation: ");
        debugPrintNUMBER(Real_uS);
        debugPrintTXT(" uS passed. ");
        debugPrintTXT("Around: ");
        debugPrintNUMBER(SID_uS);
        debugPrintTXT(" uS per irq, ");
        debugPrintTXT("at multiplier: ");
        debugPrintNUMBER(multiplier);
        debugPrintTXT(". Estimated emulated IRQ time: ");
        debugPrintNUMBER(estimated_frame_time);
        debugPrintTXTln("uS.");
    */
    multiplier++;
    if (multiplier > 248) {
      multiplier = 248;
      best_multiplier = multiplier; // if it's not found by now, then take worst case.
    }
  }
  multiplier = best_multiplier;


  // calculate time of one emulated frame

  debugPrintTXTln  (" ");
  debugPrintTXTln  ("SID emulation only:");
  debugPrintTXT    ("IRQ time:                            ");
  debugPrintNUMBER (SID_uS);
  debugPrintTXTln  (" uS ");
  debugPrintTXT    ("Calculated optimal multiplier:       ");
  debugPrintNUMBER (multiplier);
  debugPrintTXTln  (" (uS) ");
  debugPrintTXT    ("Samplerate:                          ");
  debugPrintNUMBER (uint32_t(1000000 / multiplier));
  debugPrintTXTln  (" Hz ");

  if (multiplier < 12) {
    period = multiplier;
  }
  else {
    period = 4; // for now, , maybe i'll leave at starting value (but it must be less then multiplier)
  }

  // SID emulation + 6502 CPU emulation debug

  Init6502(); // Emulator/player initialization (located in inside 6502.ino )
  InitHardware(); // Setup timers and interrupts





  Emu_uS = 0;
  Real_uS = 0;
  instructions = 0;

  Real_uS_start = micros();
  for (uint32_t total_instructions = 0; total_instructions < test_instructions; total_instructions++) {

    exec6502(); // execute 1 instruction
    Emu_uS = Emu_uS + (ticktable[opcode]);
    SID_Emulator();
  }// instrctuin loop
  Real_uS_end = micros();
  Real_uS = Real_uS_end - Real_uS_start;
  CPU_uS = (Real_uS / test_instructions) - multiplier;



  debugPrintTXTln  (" ");
  debugPrintTXTln  ("SID emulation + 6502 CPU emulation:  ");
  debugPrintTXT    ("Instructions executed:               ");
  debugPrintNUMBER(instructions);
  debugPrintTXTln  (" ");
  debugPrintTXT    ("Real time passed:                    ");
  debugPrintNUMBER (Real_uS);
  debugPrintTXTln  (" uS");
  debugPrintTXT    ("Emulated time passed:                ");
  debugPrintNUMBER (Emu_uS);
  debugPrintTXTln  (" uS");
  debugPrintTXT    ("Emulated CPU speed:                  ");
  debugPrintNUMBER (uint32_t((100 * Emu_uS) / Real_uS ));
  debugPrintTXTln  ("%");
  debugPrintTXT    ("Estimated time per CPU instruction:  ");
  debugPrintNUMBER (CPU_uS );
  debugPrintTXTln  (" uS");
  debugPrintTXTln  (" ");

  reset6502();
  Real_uS_start = micros();
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
    // test point - function overhead
    //    while (STAD4XX == 1) {} // was $d404,$d40b,$d412,$d418 accessed?
    // it can be left out if <multiplier> is small enough, so no more then 4 or 5 6502 instruction is executed between irq's

  }

  Real_uS_end = micros();

  Real_uS = Real_uS_end - Real_uS_start;
  debugPrintTXT    ("Instructions executed:               ");
  debugPrintNUMBER (instructions);
  debugPrintTXTln  (" ");
  debugPrintTXT    ("Frame time (real):                   ");
  debugPrintNUMBER (Real_uS);
  debugPrintTXTln  ("uS ");
  debugPrintTXT    ("Frame time (emulated):               ");
  debugPrintNUMBER(Emu_uS);
  debugPrintTXTln("uS ");

  debugPrintTXTln (" ");
  debugPrintTXTln("Test ended");
  Emu_uS = 0;
  Real_uS = 0;

  // calculate free unused RAM

  debugPrintTXTln  (" ");
  debugPrintTXT    ("    **** STM32 SID PLAYER ****    ");
  debugPrintTXTln  (" ");
  debugPrintTXT    ("  ");
  debugPrintNUMBER ((RAM_SIZE >> 10) + 1);
  debugPrintTXT    ("K RAM SYSTEM  ");
  debugPrintNUMBER (FreeBytes());
  debugPrintTXT    (" BYTES FREE");
  debugPrintTXTln  (" ");
  debugPrintTXTln  (" ");
  debugPrintTXTln  ("READY.");

} //


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main loop
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  // Player routine


  exec6502(); // execute 1 instruction
  Emu_uS = Emu_uS + (ticktable[opcode]); // get emulated uS value

  //    while (STAD4XX == 1) {} // was $d404,$d40b,$d412,$d418 accessed?
  // it can be left out if <multiplier> is small enough, so no more then 4 or 5 6502 instruction is executed between irq's
  if (JSR1003 == 1) { // JSR1003 check
    JSR1003 = 0;

    if (VIC_irq_request == 0) {

      /*
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
      Emu_uS = 0;

    }
    while (VIC_irq_request == 0) { // wait untill irq say it's new frame
      ///////////////////////////////////////
      //
      // insert code here, not longer then 7mS execution time
      //
      ///////////////////////////////////////

    }
    //

    Real_uS_start = micros();

    VIC_irq_request = 0;
    instructions = 0;

    if (play_next_tune == true) {
      reset6502();
      // resetSID();
      RAM[0x0304] = SID_current_tune - 1 ; // set song number (-1 offset since in sid header tunes are 1 indexed, in player it's 0 indexed)
      play_next_tune = false;
    }

  } // JSR1003 check

}// loop
