


inline void set_tune_speed () { // set tune speed best on IRQ_TYPE_PER_TUNE and VIDEO_TYPE
  SUBTUNE_SPEED = (IRQ_TYPE_PER_TUNE >> ((SID_current_tune - 1) & 0x1f)) & 0x01;
  // this need to be done per tune
  // 0 - VBI
  // 1 - CIA

  // VIDEO_TYPE
  //  00 = Unknown, //
  //  01 = PAL,
  //  10 = NTSC,
  //  11 = PAL and NTSC. // hmm, i think i'll set PAL CIA's speed for this one.

  // make it a switch to make life easier

  switch ((VIDEO_TYPE << 1) | SUBTUNE_SPEED)  {
    case 0: // 000 // VBI - UNKNOWN
      SID_speed = 19968; // assume it's pal
      break;

    case 1: // 001 // CIA - UNKNOWN
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667;
      }

      break;

    case 2: // 010 // VBI - PAL
      SID_speed = 19968;
      break;

    case 3: // 011 // CIA - PAL
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667;
      }

      break;

    case 4: // 100 // VBI - NTSC
      SID_speed = 17045;
      break;

    case 5: // 101 // CIA - NTSC
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 17095;
      }

      break;

    case 6: // 110 // VBI - PAL and NTSC
      SID_speed = 19968; // assume it's pal vbi
      break;

    case 7: // 111 // CIA - PAL and NTSC
      if (CIA_DC05 > 0) {
        SID_speed = (CIA_DC05 << 8) + CIA_DC04;
      }
      else {
        SID_speed = 16667; // assume it's pal cia
      }

      break;


  }

}


inline bool Compatibility_check() {

  MagicID = PEEK (0 +  0x0380) ;
  VERSION =  PEEK (0x05 +  0x0380);
  RAM_OVERFLOW = false;
  if ( ( SID_data_size + 0x0400 - 0x7e) > RAM_SIZE    ) {
    RAM_OVERFLOW = true;

  }

  LOAD_ADDRESS = 0 ;
  SID_load_start = (PEEK (0x08 +  0x0380) * 256) + (PEEK (0x09 +  0x0380));

  if (SID_load_start == 0) {
    SID_load_start = PEEK (0x7c + 0x380) + (PEEK (0x7d + 0x380) * 256);
  }
  if ( SID_load_start >= 0x0400)  { // >= 0x07E8)  {
    LOAD_ADDRESS = SID_load_start;
  }

  SID_load_end = SID_load_start + SID_data_size - 0x7e ;
  if ( (RAM_SIZE ) < (SID_load_end ) ) {
    LOW_RAM = true;

  }
  else {
    LOW_RAM = false;

  }

  SID_play =  PEEK (0x0d + 0x0380) + (PEEK (0x0c + 0x0380) * 256);
  SID_init = PEEK (0x0b + 0x0380) + (PEEK (0x0a + 0x0380) * 256);
  SID_number_of_tunes =  PEEK (0x0f + 0x0380) + ( PEEK (0x0e + 0x0380) * 256);
  SID_default_tune =  PEEK (0x11 + 0x0380) + ( PEEK (0x10 + 0x0380) * 256);
  SID_current_tune =  SID_default_tune;



  // $12-$15 - 32bit big endian bits
  // 0 - VBI , 1 - CIA
  // for tunes > 32 , it's tune&0x1f
  // it is just indication what type of interrupts is used: VIC or CIA, and only default values are used.
  // Multispeed tune's  code set it's own VIC/CIA values. Must emulate VIC and CIA to be able to play(detect) multi speed tunes.
  IRQ_TYPE_PER_TUNE = (      ( (PEEK (0x15 + 0x0380)) )
                             |      ( (PEEK (0x14 + 0x0380)) << 8 )
                             |                          ( (PEEK (0x13 + 0x0380)) << 16 )
                             |                          ( (PEEK (0x12 + 0x0380)) << 24 )
                      );



  FLAGS76 = ( ( (PEEK (0x76 + 0x0380)) << 8 ) | (PEEK (0x77 + 0x0380)) ); // 16bit big endian number

  ComputeMUSplayer = FLAGS76 & 0x01;                        // bit0 - if set, not playable

  C64Compatible = (FLAGS76 >> 1) & 0x01;                   // bit1 - is PlaySID specific, e.g. uses PlaySID samples (psidSpecific):
  //                                                          0 = C64 compatible,                            // playable
  //                                                          1 = PlaySID specific (PSID v2NG, v3, v4)       // not playable
  //                                                          1 = C64 BASIC flag (RSID)                      // not playable

  VIDEO_TYPE =  (FLAGS76 >> 2) & 0x03 ;                       // 2bit value
  //  00 = Unknown,
  //  01 = PAL,
  //  10 = NTSC,
  //  11 = PAL and NTSC.
  // used in combination with SUBTUNE_SPEED to set speed per tune


  set_tune_speed(); // set SID_speed

  MODEL_TYPE =  (FLAGS76 >> 4) & 0x03 ;                       // SID Model
  // 00 = Unknown,
  // 01 = MOS6581,
  // 10 = MOS8580,
  // 11 = MOS6581 and MOS8580.

  //
  LOW_RAM_uncompatible = false;
  if (LOW_RAM) {
    if ( (SID_play < SID_load_start) | (SID_play > SID_load_end) ) {
      LOW_RAM_uncompatible = true;

    }
  }

  if (
    (MagicID != 0x50) /*short version */
    | (RAM_OVERFLOW)
    | (VERSION < 2)
    | (SID_play == 0)
    | (ComputeMUSplayer)
    | (C64Compatible)
    | (LOW_RAM_uncompatible) )
  { // play tune if no errors
    PLAYABLE_SID = false;

  }
  else {
    PLAYABLE_SID = true;

  }
  return PLAYABLE_SID;
}

inline void player_setup() {

  for ( i = 0; i < 0x80; i++) {
    POKE (i + 0x0300, MyROM[i] ); //
  }
  POKE (0x0304, SID_current_tune - 1 );
  POKE (0x0307, (SID_init >> 8) & 0xff);
  POKE (0x0306, SID_init & 0xff);
  POKE (0x0310, (SID_play >> 8) & 0xff);
  POKE (0x030f, SID_play & 0xff);

}
