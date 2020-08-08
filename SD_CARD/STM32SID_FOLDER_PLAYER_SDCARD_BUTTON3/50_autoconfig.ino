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
    estimated_frame_time = ((20000 /* ms */  / multiplier) * (SID_uS + 2 /* irq overhead */ )) + (500 /* estimated irq instructions */ * (CPU_uS + 0 /* estimated main loop overhead in uS */  )); // estimated around 500 instruction per frame (20mS) should be enough for calculations
    if (estimated_frame_time < 13000) { // worst case: 13000 for bluepill at 48MHz, O0 (smallest code) optimatization (best case is same as emulated uS)
      best_multiplier = multiplier ;
    }
    // enable this if raw values are needed during calculations (in case to manualy find perfect <multiplier> value )

//             debugPrintTXTln("Testing SID Emulator");
//             debugPrintTXT("SID emulation: ");
//             debugPrintNUMBER(Real_uS);
//             debugPrintTXT(" uS passed. ");
//             debugPrintTXT("Around: ");
//             debugPrintNUMBER(SID_uS);
//             debugPrintTXT(" uS per irq, ");
//             debugPrintTXT("at multiplier: ");
//             debugPrintNUMBER(multiplier);
//             debugPrintTXT(". Estimated emulated IRQ time: ");
//             debugPrintNUMBER(estimated_frame_time);
//             debugPrintTXTln("uS.");

    multiplier++;
    if (multiplier > 248) {
      multiplier = 248;
      best_multiplier = multiplier; // if it's not found by now, then take worst case.
    }
  }
  multiplier = best_multiplier; // or edit manually here

  debugPrintTXT    ("SID emulator IRQ:  ");  debugPrintNUMBER (SID_uS);  debugPrintTXTln  (" uS ");
  debugPrintTXT    ("Optimal multiplier:");  debugPrintNUMBER (multiplier);  debugPrintTXTln  (" (uS) ");
  debugPrintTXT    ("Samplerate:        ");  debugPrintNUMBER (uint32_t(1000000 / multiplier));  debugPrintTXTln  (" Hz ");

  if (multiplier < 12) {
    period = multiplier;
  }
  else {
    period = 4; // for now, , maybe i'll leave at starting value (but it must be less then multiplier)
  }

}

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
