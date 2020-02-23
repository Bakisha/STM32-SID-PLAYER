// Fake6502 emulator
// http://rubbermallet.org/fake6502.c

// 6502 emulation
// far from perfect, it runs very slow, once interrupts are enabled, compared to 1Mhz CPU, but it is enough to do the job (emulation is needed only in few raster lines only anyway, around 500 instructions)
// I only edited part with read and write so it can act uppon addresses


inline  uint8_t read6502(uint16_t address) {

  return_value = 0x60; // if it's not in any of knowed area, assume it's 0x60 (RTS) (Dan Dare 3 fix)

  if (address == 0x030c) { // player's  sid-play subroutine's adress
    JSR1003 = 1; // if it's loading <20-03-10> (example: JSR $1003) value from sid_play routine, it is time for fake VIC-II irq signal
  }
  if ((address >= 0xD400) && (address < 0xD420)) {
   
    STAD4XX = 1; // sid read
    return_value = SID[address - 0xD400]; // TODO: make it unreadable //   SID
  }

  if ( (address >=  SID_load_start)  & (address <  SID_load_end ) ) { // sid player area

    if ( LOW_RAM == true ) { // hack to relocate addresses from SID_start to 0x400

      if ( (0x400 + address - SID_load_start) < (RAM_SIZE )) { // read from RAM if it's in available RAM,
        return_value = PEEK (0x400 + address - SID_load_start) ;
      }

    }

  }


  if ( LOW_RAM == false ) {
    if (address <= (RAM_SIZE )) return_value = PEEK (address) ; // for  memory space that is covered by RAM

  }

  if ( address < (0x400) ) return_value = PEEK (address) ; // zero page, stack, player, screen RAM

//CIA timer
  if ( address == 0xdc04 )  {
  return_value = CIA_DC04;
}
if ( address == 0xdc05 )  {
  return_value = CIA_DC05;
}





return return_value;

}


inline void write6502(uint16_t address, uint8_t value) {



  if ((address >= 0xD400) & (address < 0xD420)) { // writing to sid - assagn values for interrupt
    //STAD4XX = 1; // SID write signal for IRQ

    // SID MAGIC
    access_adress = (address - 0xD400);
    SID[ (access_adress)] = value; //  SID


    // disable if IRQ is transfering SID[] variable
    switch (access_adress) {

      case 0:
        OSC_1_HiLo = ((SID[0] & 0xff) + ( (SID[1] & 0xff) << 8) ); // *0.985
        break;
      case 1:
        OSC_1_HiLo = ((SID[0] & 0xff) + ( (SID[1] & 0xff) << 8)); // *0.985
        break;
      case 2:
        PW_HiLo_voice_1 = SID[2] + (((SID[3] & 0x0f) << 8 ));
        break;
      case 3:
        PW_HiLo_voice_1 = SID[2] + (( (SID[3] & 0x0f) << 8 ));
        break;
      case 4:
        STAD4XX = 1;
        noise_bit_voice_1 = ( (SID[4] >> 7 ) & 1) ;
        pulse_bit_voice_1 = ( (SID[4] >> 6 ) & 1) ;
        sawtooth_bit_voice_1 = ( (SID[4] >> 5 ) & 1) ;
        triangle_bit_voice_1 = ( (SID[4] >> 4 ) & 1) ;
        test_bit_voice_1   = ( (SID[4] >> 3 ) & 1) ; //
        ring_bit_voice_1   = ( (SID[4] >> 2 ) & 1) ;
        SYNC_bit_voice_1 = ( (SID[4] >> 1 ) & 1) ; //
        Gate_bit_1 = SID[4] & 1;   //

        //waveform_switch_1 = (noise_bit_voice_1 << 3) | (pulse_bit_voice_1 << 2) | (sawtooth_bit_voice_1 << 1) | (triangle_bit_voice_1); // for barebone version
        //waveform_switch_1 =  ( SID[4] >> 4); // it's in IRQ

        break;
      case 5:
        ADSR_Attack_1 = ( (SID[5] >> 4 ) & 0x0f) ;
        ADSR_Decay_1 = ( (SID[5]  ) & 0x0f) ;
        break;
      case 6:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_1 = ( (SID[6] >> 4 ) & 0x0f) ;
        ADSR_Release_1 = ( (SID[6]  ) & 0x0f);
        break;
      case 7:
        OSC_2_HiLo = (SID[7] + ( SID[8] << 8)) ; // PAL speed recalc // *0.985
        break;
      case 8:
        OSC_2_HiLo = (SID[7] + ( SID[8] << 8)) ; // *0.985
        break;
      case 9:
        PW_HiLo_voice_2 = SID[9] + ( (SID[10] & 0x0f) << 8);
        break;
      case 10:
        PW_HiLo_voice_2 = SID[9] + ( (SID[10] & 0x0f) << 8);
        break;
      case 11:
        STAD4XX = 1;
        noise_bit_voice_2 = ( (SID[11] >> 7 ) & 1) ;
        pulse_bit_voice_2 = ( (SID[11] >> 6 ) & 1) ;
        sawtooth_bit_voice_2 = ( (SID[11] >> 5 ) & 1) ;
        triangle_bit_voice_2 = ( (SID[11] >> 4 ) & 1) ;
        test_bit_voice_2   = ( (SID[11] >> 3 ) & 1) ; //
        ring_bit_voice_2 = ( (SID[11] >> 2 ) & 1) ; //
        SYNC_bit_voice_2 = ( (SID[11] >> 1 ) & 1) ; //
        Gate_bit_2 = SID[11] & 1;   //

        //waveform_switch_2 = (noise_bit_voice_2 << 3) | (pulse_bit_voice_2 << 2) | (sawtooth_bit_voice_2 << 1) | (triangle_bit_voice_2);
        //waveform_switch_2 = 0x0f & ( SID[11] >> 4);

        break;
      case 12:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Attack_2 = ( (SID[12] >> 4 ) & 0x0f) ;
        ADSR_Decay_2 = ( (SID[12]  ) & 0x0f) ;
        break;
      case 13:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_2 = ( (SID[13] >> 4 ) & 0x0f) ;
        ADSR_Release_2 = ( (SID[13]  ) & 0x0f);
        break;
      case 14:
        OSC_3_HiLo = (SID[14] + ( SID[15] << 8)) ; // *0.985
        break;
      case 15:
        OSC_3_HiLo = (SID[14] + ( SID[15] << 8)) ; // *0.985
        break;
      case 16:
        PW_HiLo_voice_3 = SID[16] + ( (SID[17] & 0x0f) << 8);
        break;
      case 17:
        PW_HiLo_voice_3 = SID[16] + ( (SID[17] & 0x0f) << 8);
        break;
      case 18:
        STAD4XX = 1;
        noise_bit_voice_3 = ( (SID[18] >> 7 ) & 1) ;
        pulse_bit_voice_3 = ( (SID[18] >> 6 ) & 1) ;
        sawtooth_bit_voice_3 = ( (SID[18] >> 5 ) & 1) ;
        triangle_bit_voice_3 = ( (SID[18] >> 4 ) & 1) ;
        test_bit_voice_3  = ( (SID[18] >> 3 ) & 1) ; //
        ring_bit_voice_3 = ( (SID[18] >> 2 ) & 1) ; //
        SYNC_bit_voice_3 = ( (SID[18] >> 1 ) & 1) ; //
        Gate_bit_3 = SID[18] & 1;   //

        //waveform_switch_3 = (noise_bit_voice_3 << 3) | (pulse_bit_voice_3 << 2) | (sawtooth_bit_voice_3 << 1) | (triangle_bit_voice_3); // for barebone version
        //waveform_switch_3 = 0x0f & ( SID[18] >> 4);
        break;
      case 19:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Attack_3 = ( (SID[19] >> 4 ) & 0x0f) ;
        ADSR_Decay_3 = ( (SID[19]  ) & 0x0f) ;
        break;
      case 20:
        //STAD4XX = 1; // SID write signal for IRQ
        ADSR_Sustain_3 = ( (SID[20] >> 4 ) & 0x0f) ;
        ADSR_Release_3 = ( (SID[20]  ) & 0x0f);
        break;
      case 21:
        FILTER_HiLo = (SID[21] & 0x07) + ( SID[22] << 3); // 11bit // TODO

        //set w0
        // from reSID
        // Multiply with 1.048576 to facilitate division by 1 000 000 by right-
        // shifting 20 times (2 ^ 20 = 1048576).
        // w0 = static_cast<sound_sample>(2*pi*f0[fc]*1.048576);
        w0 = w0_constant_part * (FILTER_HiLo + 0x01); // 0x01 offset testing
        // w0_ceil_dt = w0 <= w0_max_dt ? w0 : w0_max_dt;
        if (w0 < w0_max_dt) {
          w0_ceil_dt = w0;
        }
        else {
          w0_ceil_dt = w0_max_dt;
        }

        break;
      case 22:
        FILTER_HiLo = (SID[21] & 0x07) + (SID[22] << 3); // 11bit // TODO

        //set w0
        // Multiply with 1.048576 to facilitate division by 1 000 000 by right-
        // shifting 20 times (2 ^ 20 = 1048576).
        // w0 = static_cast<sound_sample>(2*pi*f0[fc]*1.048576);
        w0 = w0_constant_part * (FILTER_HiLo + 0x1); //

        // w0_ceil_dt = w0 <= w0_max_dt ? w0 : w0_max_dt;
        if (w0 < w0_max_dt) {
          w0_ceil_dt = w0;
        }
        else {
          w0_ceil_dt = w0_max_dt;
        }

        break;
      case 23:

        FILTER_Resonance = ( (SID[23] >> 4 ) & 0x0f) ;; // 4bit // TODO
        FILTER_Enable_1 =  SID[23]  & 1 ; // on/off
        FILTER_Enable_2 = ( (SID[23] >> 1 ) & 1) ;; // on/off
        FILTER_Enable_3 = ( (SID[23] >> 2 ) & 1) ;; // on/off
        FILTER_Enable_EXT = ( (SID[23] >> 3 ) & 1) ;; // on/off

        FILTER_Enable_switch =  SID[23]  & 0x07 ; // for filter switch in irq (no external input filter)

        // The coefficient 1024 is dispensed of later by right-shifting 10 times
        // (2 ^ 10 = 1024).
        // _1024_div_Q = static_cast<sound_sample>(1024.0/(0.707 + 1.0*res/0x0f));
        Q_1024_div = (1024.0 / (0.707 + 1.0 * (FILTER_Resonance) / 15.0));

        break;
      case 24:
        //STAD4XX = 1; // SID write signal for IRQ
        OFF3 =  ( (SID[24] >> 7 ) & 1) ;; // on/off; //
        FILTER_HP =  ( (SID[24] >> 6 ) & 1) ;; // on/off; //;
        FILTER_BP =  ( (SID[24] >> 5 ) & 1) ;; // on/off; //;
        FILTER_LP =  ( (SID[24] >> 4 ) & 1) ;; // on/off; //;
        MASTER_VOLUME =   (SID[24]  & 15) ;; // on/off; //;
        // change volume immidiattelly
        //main_volume = MASTER_VOLUME * ( main_volume_32bit) / 15;
        //TIMER1_BASE->CCR1 =  main_volume;
        // disable if there is there is no delay hack
        //STAD4XX = 1;

        break;
      case 25:

        break;
      case 26:

        break;
      case 27:

        break;
      case 28:

        break;
      case 29:

        break;
      case 30:

        break;
      case 31:
        break;
    }

    // STAD4XX = 1; // SID write signal for IRQ
    //PB13_HIGH;
  }


  if ( LOW_RAM == true ) {
    if ( (address >=  SID_load_start)   & (address <  SID_load_end ) ) {
      if ( 0x400 + address - SID_load_start < (RAM_SIZE )) { // write to memory only if it fits into RAM, rest is ignored.
        POKE (0x400 + address - SID_load_start, value); // sid data memory space
      }
    }

    if (address < 0x400) POKE (address, value); // zero page, stack, player and free ram
  } // LOW_RAM



  if ( LOW_RAM == false ) {
    if (address <= (RAM_SIZE )) {
      POKE (address, value); // for  memory space that is covered by RAM
    }
  } // LOW_RAM


 //CIA timer
  if ( address == 0xdc04 )  {
 CIA_DC04=value;
}
if ( address == 0xdc05 )  {
  CIA_DC05=value;
}
if (CIA_DC05>0) { // set song speed only when Hi value of CIA timer is greater then 0
  set_tune_speed ();
}

}


// Blue6502's voids // i didn't change much of Fake6502's code, it's working good

inline void reset6502() {

  pc = 0x0300;  // player is copied to RAM at 0x300
  //pc = (uint16_t)read6502(0xFFFC) | ((uint16_t)read6502(0xFFFD) << 8); // reset vector not used
  a = 0;
  x = 0;
  y = 0;
  sp = 0xFD;
  cpustatus |= FLAG_CONSTANT;

  CIA_DC04 = 0;
  CIA_DC05 = 0;


}




//a few general functions used by various other functions
inline void push16(uint16_t pushval) {
  write6502(BASE_STACK + sp, (pushval >> 8) & 0xFF);
  write6502(BASE_STACK + ((sp - 1) & 0xFF), pushval & 0xFF);
  sp -= 2;
}

inline void push8(uint8_t pushval) {
  write6502(BASE_STACK + sp--, pushval);
}

inline uint16_t pull16() {

  temp16 = read6502(BASE_STACK + ((sp + 1) & 0xFF)) | (read6502(BASE_STACK + ((sp + 2) & 0xFF)) << 8);
  sp += 2;
  return (temp16);
}

inline uint8_t pull8() {
  return (read6502(BASE_STACK + ++sp));
}



//addressing mode functions, calculates effective addresses
inline void imp() { //implied
}

inline void acc() { //accumulator
  useaccum = 1;
}

inline void imm() { //immediate
  ea = pc++;
}

inline void zp() { //zero-page
  ea = read6502(pc++);
}

inline void zpx() { //zero-page,X
  ea = (read6502(pc++) + x) & 0xFF; //zero-page wraparound
}

inline void zpy() { //zero-page,Y
  ea = (read6502(pc++) + y) & 0xFF; //zero-page wraparound
}

inline void rel() { //relative for branch ops (8-bit immediate value, sign-extended)
  reladdr = read6502(pc++);
  if (reladdr & 0x80) reladdr |= 0xFF00;
}

inline void abso() { //absolute
  ea = read6502(pc) | (read6502(pc + 1) << 8);
  pc += 2;
}

inline void absx() { //absolute,X

  ea = (read6502(pc) | (read6502(pc + 1) << 8));
  startpage = ea & 0xFF00;
  ea += x;

  pc += 2;
}

inline void absy() { //absolute,Y

  ea = (read6502(pc) | (read6502(pc + 1) << 8));
  startpage = ea & 0xFF00;
  ea += y;

  pc += 2;
}

inline void ind() { //indirect

  eahelp = read6502(pc) | (read6502(pc + 1) << 8);
  eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //replicate 6502 page-boundary wraparound bug
  ea = read6502(eahelp) | (read6502(eahelp2) << 8);
  pc += 2;
}

inline void indx() { // (indirect,X)

  eahelp = ((read6502(pc++) + x) & 0xFF); //zero-page wraparound for table pointer
  ea = read6502(eahelp & 0x00FF) | (read6502((eahelp + 1) & 0x00FF) << 8);
}

inline void indy() { // (indirect),Y

  eahelp = read6502(pc++);
  eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //zero-page wraparound
  ea = read6502(eahelp) | (read6502(eahelp2) << 8);
  startpage = ea & 0xFF00;
  ea += y;

}

uint16_t getvalue() {
  if (useaccum) return (a);
  else return (read6502(ea));
}

uint16_t getvalue16() {
  return (read6502(ea) | (read6502(ea + 1) << 8));
}

inline void putvalue(uint16_t saveval) {
  if (useaccum) a = (saveval & 0x00FF);
  else write6502(ea, (saveval & 0x00FF));
}


//instruction handler functions
inline void adc() {
  value = getvalue();
  result = a + value + (cpustatus & FLAG_CARRY);

  carrycalc(result);
  zerocalc(result);
  overflowcalc(result, a, value);
  signcalc(result);
  saveaccum(result);
}


inline void op_and() {
  value = getvalue();
  result = a & value;

  zerocalc(result);
  signcalc(result);

  saveaccum(result);
}

inline void asl() {
  value = getvalue();
  result = value << 1;

  carrycalc(result);
  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void bcc() {
  if ((cpustatus & FLAG_CARRY) == 0) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bcs() {
  if ((cpustatus & FLAG_CARRY) == FLAG_CARRY) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void beq() {
  if ((cpustatus & FLAG_ZERO) == FLAG_ZERO) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void op_bit() {
  value = getvalue();
  result = a & value;

  zerocalc(result);
  cpustatus = (cpustatus & 0x3F) | (value & 0xC0);
}

inline void bmi() {
  if ((cpustatus & FLAG_SIGN) == FLAG_SIGN) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bne() {
  if ((cpustatus & FLAG_ZERO) == 0) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bpl() {
  if ((cpustatus & FLAG_SIGN) == 0) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void brk() {
  pc++;
  push16(pc); //push next instruction address onto stack
  push8(cpustatus | FLAG_BREAK); //push CPU cpustatus to stack
  setinterrupt(); //set interrupt flag
  pc = read6502(0xFFFE) | (read6502(0xFFFF) << 8);
}

inline void bvc() {
  if ((cpustatus & FLAG_OVERFLOW) == 0) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void bvs() {
  if ((cpustatus & FLAG_OVERFLOW) == FLAG_OVERFLOW) {
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
    else clockticks6502++;
  }
}

inline void clc() {
  clearcarry();
}

inline void cld() {
  cleardecimal();
}

inline void cli() {
  clearinterrupt();
}

inline void clv() {
  clearoverflow();
}

inline void cmp() {
  value = getvalue();
  result = a - value;

  if (a >= (value & 0x00FF)) setcarry();
  else clearcarry();
  if (a == (value & 0x00FF)) setzero();
  else clearzero();
  signcalc(result);
}

inline void cpx() {
  value = getvalue();
  result = x - value;

  if (x >= (value & 0x00FF)) setcarry();
  else clearcarry();
  if (x == (value & 0x00FF)) setzero();
  else clearzero();
  signcalc(result);
}

inline void cpy() {
  value = getvalue();
  result = y - value;

  if (y >= (value & 0x00FF)) setcarry();
  else clearcarry();
  if (y == (value & 0x00FF)) setzero();
  else clearzero();
  signcalc(result);
}

inline void dec() {
  value = getvalue();
  result = value - 1;

  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void dex() {
  x--;

  zerocalc(x);
  signcalc(x);
}

inline void dey() {
  y--;

  zerocalc(y);
  signcalc(y);
}

inline void eor() {
  value = getvalue();
  result = a ^ value;

  zerocalc(result);
  signcalc(result);

  saveaccum(result);
}

inline void inc() {
  value = getvalue();
  result = value + 1;

  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void inx() {
  x++;

  zerocalc(x);
  signcalc(x);
}

inline void iny() {
  y++;

  zerocalc(y);
  signcalc(y);
}

inline void jmp() {
  pc = ea;
}

inline void jsr() {
  push16(pc - 1);
  pc = ea;
}

inline void lda() {
  value = getvalue();
  a = (value & 0x00FF);

  zerocalc(a);
  signcalc(a);
}

inline void ldx() {
  value = getvalue();
  x = (value & 0x00FF);

  zerocalc(x);
  signcalc(x);
}

inline void ldy() {
  value = getvalue();
  y = (value & 0x00FF);

  zerocalc(y);
  signcalc(y);
}

inline void lsr() {
  value = getvalue();
  result = value >> 1;

  if (value & 1) setcarry();
  else clearcarry();
  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void nop() {
}

inline void ora() {
  value = getvalue();
  result = a | value;

  zerocalc(result);
  signcalc(result);

  saveaccum(result);
}

inline void pha() {
  push8(a);
}

inline void php() {
  push8(cpustatus | FLAG_BREAK);
}

inline void pla() {
  a = pull8();

  zerocalc(a);
  signcalc(a);
}

inline void plp() {
  cpustatus = pull8() | FLAG_CONSTANT;
}

inline void rol() {
  value = getvalue();
  result = (value << 1) | (cpustatus & FLAG_CARRY);

  carrycalc(result);
  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void ror() {
  value = getvalue();
  result = (value >> 1) | ((cpustatus & FLAG_CARRY) << 7);

  if (value & 1) setcarry();
  else clearcarry();
  zerocalc(result);
  signcalc(result);

  putvalue(result);
}

inline void rti() {
  cpustatus = pull8();
  value = pull16();
  pc = value;
}

inline void rts() {
  value = pull16();
  pc = value + 1;
}

inline void sbc() {
  value = getvalue() ^ 0x00FF;
  result = a + value + (cpustatus & FLAG_CARRY);

  carrycalc(result);
  zerocalc(result);
  overflowcalc(result, a, value);
  signcalc(result);



  saveaccum(result);
}


inline void sec() {
  setcarry();
}

inline void sed() {
  setdecimal();
}

inline void sei() {
  setinterrupt();
}

inline void sta() {
  putvalue(a);
}

inline void stx() {
  putvalue(x);
}

inline void sty() {
  putvalue(y);
}

inline void tax() {
  x = a;

  zerocalc(x);
  signcalc(x);
}

inline void tay() {
  y = a;

  zerocalc(y);
  signcalc(y);
}

inline void tsx() {
  x = sp;

  zerocalc(x);
  signcalc(x);
}

inline void txa() {
  a = x;

  zerocalc(a);
  signcalc(a);
}

inline void txs() {
  sp = x;
}

inline void tya() {
  a = y;

  zerocalc(a);
  signcalc(a);
}

//undocumented instructions
#ifdef UNDOCUMENTED
inline void lax() {
  lda();
  ldx();
}

inline void sax() {
  sta();
  stx();
  putvalue(a & x);
}

inline void dcp() {
  dec();
  cmp();
}

inline void isb() {
  inc();
  sbc();
}

inline void slo() {
  asl();
  ora();
}

inline void rla() {
  rol();
  op_and();
}

inline void sre() {
  lsr();
  eor();
}

inline void rra() {
  ror();
  adc();
}
#else
#define lax nop
#define sax nop
#define dcp nop
#define isb nop
#define slo nop
#define rla nop
#define sre nop
#define rra nop
#endif


inline void exec6502() {



  opcode = read6502(pc++);

  cpustatus |= FLAG_CONSTANT;

  useaccum = 0;

  switch (opcode) {
    case 0x0:
      imp();
      brk();
      break;
    case 0x1:
      indx();
      ora();
      break;
    case 0x5:
      zp();
      ora();
      break;
    case 0x6:
      zp();
      asl();
      break;
    case 0x8:
      imp();
      php();
      break;
    case 0x9:
      imm();
      ora();
      break;
    case 0xA:
      acc();
      asl();
      break;
    case 0xD:
      abso();
      ora();
      break;
    case 0xE:
      abso();
      asl();
      break;
    case 0x10:
      rel();
      bpl();
      break;
    case 0x11:
      indy();
      ora();
      break;
    case 0x15:
      zpx();
      ora();
      break;
    case 0x16:
      zpx();
      asl();
      break;
    case 0x18:
      imp();
      clc();
      break;
    case 0x19:
      absy();
      ora();
      break;
    case 0x1D:
      absx();
      ora();
      break;
    case 0x1E:
      absx();
      asl();
      break;
    case 0x20:
      abso();
      jsr();
      break;
    case 0x21:
      indx();
      op_and();
      break;
    case 0x24:
      zp();
      op_bit();
      break;
    case 0x25:
      zp();
      op_and();
      break;
    case 0x26:
      zp();
      rol();
      break;
    case 0x28:
      imp();
      plp();
      break;
    case 0x29:
      imm();
      op_and();
      break;
    case 0x2A:
      acc();
      rol();
      break;
    case 0x2C:
      abso();
      op_bit();
      break;
    case 0x2D:
      abso();
      op_and();
      break;
    case 0x2E:
      abso();
      rol();
      break;
    case 0x30:
      rel();
      bmi();
      break;
    case 0x31:
      indy();
      op_and();
      break;
    case 0x35:
      zpx();
      op_and();
      break;
    case 0x36:
      zpx();
      rol();
      break;
    case 0x38:
      imp();
      sec();
      break;
    case 0x39:
      absy();
      op_and();
      break;
    case 0x3D:
      absx();
      op_and();
      break;
    case 0x3E:
      absx();
      rol();
      break;
    case 0x40:
      imp();
      rti();
      break;
    case 0x41:
      indx();
      eor();
      break;
    case 0x45:
      zp();
      eor();
      break;
    case 0x46:
      zp();
      lsr();
      break;
    case 0x48:
      imp();
      pha();
      break;
    case 0x49:
      imm();
      eor();
      break;
    case 0x4A:
      acc();
      lsr();
      break;
    case 0x4C:
      abso();
      jmp();
      break;
    case 0x4D:
      abso();
      eor();
      break;
    case 0x4E:
      abso();
      lsr();
      break;
    case 0x50:
      rel();
      bvc();
      break;
    case 0x51:
      indy();
      eor();
      break;
    case 0x55:
      zpx();
      eor();
      break;
    case 0x56:
      zpx();
      lsr();
      break;
    case 0x58:
      imp();
      cli();
      break;
    case 0x59:
      absy();
      eor();
      break;
    case 0x5D:
      absx();
      eor();
      break;
    case 0x5E:
      absx();
      lsr();
      break;
    case 0x60:
      imp();
      rts();
      break;
    case 0x61:
      indx();
      adc();
      break;
    case 0x65:
      zp();
      adc();
      break;
    case 0x66:
      zp();
      ror();
      break;
    case 0x68:
      imp();
      pla();
      break;
    case 0x69:
      imm();
      adc();
      break;
    case 0x6A:
      acc();
      ror();
      break;
    case 0x6C:
      ind();
      jmp();
      break;
    case 0x6D:
      abso();
      adc();
      break;
    case 0x6E:
      abso();
      ror();
      break;
    case 0x70:
      rel();
      bvs();
      break;
    case 0x71:
      indy();
      adc();
      break;
    case 0x75:
      zpx();
      adc();
      break;
    case 0x76:
      zpx();
      ror();
      break;
    case 0x78:
      imp();
      sei();
      break;
    case 0x79:
      absy();
      adc();
      break;
    case 0x7D:
      absx();
      adc();
      break;
    case 0x7E:
      absx();
      ror();
      break;
    case 0x81:
      indx();
      sta();
      break;
    case 0x84:
      zp();
      sty();
      break;
    case 0x85:
      zp();
      sta();
      break;
    case 0x86:
      zp();
      stx();
      break;
    case 0x88:
      imp();
      dey();
      break;
    case 0x8A:
      imp();
      txa();
      break;
    case 0x8C:
      abso();
      sty();
      break;
    case 0x8D:
      abso();
      sta();
      break;
    case 0x8E:
      abso();
      stx();
      break;
    case 0x90:
      rel();
      bcc();
      break;
    case 0x91:
      indy();
      sta();
      break;
    case 0x94:
      zpx();
      sty();
      break;
    case 0x95:
      zpx();
      sta();
      break;
    case 0x96:
      zpy();
      stx();
      break;
    case 0x98:
      imp();
      tya();
      break;
    case 0x99:
      absy();
      sta();
      break;
    case 0x9A:
      imp();
      txs();
      break;
    case 0x9D:
      absx();
      sta();
      break;
    case 0xA0:
      imm();
      ldy();
      break;
    case 0xA1:
      indx();
      lda();
      break;
    case 0xA2:
      imm();
      ldx();
      break;
    case 0xA4:
      zp();
      ldy();
      break;
    case 0xA5:
      zp();
      lda();
      break;
    case 0xA6:
      zp();
      ldx();
      break;
    case 0xA8:
      imp();
      tay();
      break;
    case 0xA9:
      imm();
      lda();
      break;
    case 0xAA:
      imp();
      tax();
      break;
    case 0xAC:
      abso();
      ldy();
      break;
    case 0xAD:
      abso();
      lda();
      break;
    case 0xAE:
      abso();
      ldx();
      break;
    case 0xB0:
      rel();
      bcs();
      break;
    case 0xB1:
      indy();
      lda();
      break;
    case 0xB4:
      zpx();
      ldy();
      break;
    case 0xB5:
      zpx();
      lda();
      break;
    case 0xB6:
      zpy();
      ldx();
      break;
    case 0xB8:
      imp();
      clv();
      break;
    case 0xB9:
      absy();
      lda();
      break;
    case 0xBA:
      imp();
      tsx();
      break;
    case 0xBC:
      absx();
      ldy();
      break;
    case 0xBD:
      absx();
      lda();
      break;
    case 0xBE:
      absy();
      ldx();
      break;
    case 0xC0:
      imm();
      cpy();
      break;
    case 0xC1:
      indx();
      cmp();
      break;
    case 0xC4:
      zp();
      cpy();
      break;
    case 0xC5:
      zp();
      cmp();
      break;
    case 0xC6:
      zp();
      dec();
      break;
    case 0xC8:
      imp();
      iny();
      break;
    case 0xC9:
      imm();
      cmp();
      break;
    case 0xCA:
      imp();
      dex();
      break;
    case 0xCC:
      abso();
      cpy();
      break;
    case 0xCD:
      abso();
      cmp();
      break;
    case 0xCE:
      abso();
      dec();
      break;
    case 0xD0:
      rel();
      bne();
      break;
    case 0xD1:
      indy();
      cmp();
      break;
    case 0xD5:
      zpx();
      cmp();
      break;
    case 0xD6:
      zpx();
      dec();
      break;
    case 0xD8:
      imp();
      cld();
      break;
    case 0xD9:
      absy();
      cmp();
      break;
    case 0xDD:
      absx();
      cmp();
      break;
    case 0xDE:
      absx();
      dec();
      break;
    case 0xE0:
      imm();
      cpx();
      break;
    case 0xE1:
      indx();
      sbc();
      break;
    case 0xE4:
      zp();
      cpx();
      break;
    case 0xE5:
      zp();
      sbc();
      break;
    case 0xE6:
      zp();
      inc();
      break;
    case 0xE8:
      imp();
      inx();
      break;
    case 0xE9:
      imm();
      sbc();
      break;
    case 0xEB:
      imm();
      sbc();
      break;
    case 0xEC:
      abso();
      cpx();
      break;
    case 0xED:
      abso();
      sbc();
      break;
    case 0xEE:
      abso();
      inc();
      break;
    case 0xF0:
      rel();
      beq();
      break;
    case 0xF1:
      indy();
      sbc();
      break;
    case 0xF5:
      zpx();
      sbc();
      break;
    case 0xF6:
      zpx();
      inc();
      break;
    case 0xF8:
      imp();
      sed();
      break;
    case 0xF9:
      absy();
      sbc();
      break;
    case 0xFD:
      absx();
      sbc();
      break;
    case 0xFE:
      absx();
      inc();
      break;
  }

  instructions++;

}


uint16_t getpc() {
  return (pc);
}

uint8_t getop() {
  return (opcode);
}
// end of Blue6502's voids
