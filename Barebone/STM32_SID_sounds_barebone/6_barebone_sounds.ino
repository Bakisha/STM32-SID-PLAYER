
void error_sound_SD() {

  //reset_SID();

  OSC_1_HiLo = 0xffff; // just having fun with globals here :-)
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x09;
  ADSR_Decay_1  = 0x07;
  ADSR_Sustain_1 = 0x06;
  ADSR_Release_1 = 0x0b;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 0;
  triangle_bit_voice_1 = 1;
  pulse_bit_voice_1 = 1;
  noise_bit_voice_1 = 0;

  Gate_bit_1 = 1;
  delay(480);
  OSC_1_HiLo = 0xf000;
  Gate_bit_1 = 0;
  delay(4000);
}



inline void error_sound_ROOT() {

  //reset_SID();

  OSC_1_HiLo = 0x1000;
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x09;
  ADSR_Decay_1  = 0x07;
  ADSR_Sustain_1 = 0x06;
  ADSR_Release_1 = 0x0b;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 0;
  triangle_bit_voice_1 = 1;
  pulse_bit_voice_1 = 1;
  noise_bit_voice_1 = 0;


  Gate_bit_1 = 1;
  delay(480);
  OSC_1_HiLo = 0x0800;
  Gate_bit_1 = 0;
  delay(1000);

}


inline void error_open_file() {

  //reset_SID();

  OSC_1_HiLo = 0xc000;
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x00;
  ADSR_Decay_1  = 0x00;
  ADSR_Sustain_1 = 0x0f;
  ADSR_Release_1 = 0x05;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 0;
  triangle_bit_voice_1 = 0;
  pulse_bit_voice_1 = 0;
  noise_bit_voice_1 = 1;

  Gate_bit_1 = 1;
  delay(480);
  OSC_1_HiLo = 0xc800;
  Gate_bit_1 = 0;
  delay(480);

}


inline void error_open_folder () {

  //reset_SID();

  OSC_1_HiLo = 0x2000;
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x09;
  ADSR_Decay_1  = 0x07;
  ADSR_Sustain_1 = 0x06;
  ADSR_Release_1 = 0x0b;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 1;
  triangle_bit_voice_1 = 1;
  pulse_bit_voice_1 = 1;
  noise_bit_voice_1 = 0;

  Gate_bit_1 = 1;
  delay(480);
  OSC_1_HiLo = 0x1000;
  Gate_bit_1 = 0;

  delay(1000);
}

inline void error_open_sid () {

  //reset_SID();

  OSC_1_HiLo = 0x4000;
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x0b;
  ADSR_Decay_1  = 0x08;
  ADSR_Sustain_1 = 0x06;
  ADSR_Release_1 = 0x0b;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 1;
  triangle_bit_voice_1 = 0;
  pulse_bit_voice_1 = 1;
  noise_bit_voice_1 = 0;
  
  Gate_bit_1 = 1;
  delay(1500);
  OSC_1_HiLo = 0x3f00;
  Gate_bit_1 = 0;
  Gate_bit_2 = 0;
  Gate_bit_3 = 0;
  for (int oscup = 0; oscup > 4000; oscup++) {
    OSC_1_HiLo = oscup;
    delay(1);
  }
}

inline void error_PSID_V2_RAM_OVERFLOW () {

  //reset_SID();

  OSC_1_HiLo = 0x4000; // barebone sound
  MASTER_VOLUME  = 0x0f;
  ADSR_Attack_1  = 0x0b;
  ADSR_Decay_1  = 0x08;
  ADSR_Sustain_1 = 0x06;
  ADSR_Release_1 = 0x0b;
  PW_HiLo_voice_1 = 0x400;

  sawtooth_bit_voice_1 = 0;
  triangle_bit_voice_1 = 1;
  pulse_bit_voice_1 = 1;
  noise_bit_voice_1 = 0;
  
  Gate_bit_1 = 1;
  delay(1500);
  OSC_1_HiLo = 0x3f00;
  Gate_bit_1 = 0;
  Gate_bit_2 = 0;
  Gate_bit_3 = 0;

  for (int oscdown = 4000; oscdown > 0; oscdown = oscdown - 2) {
    OSC_1_HiLo = oscdown;
    delay(1);
  }
}

inline void reset_SID() {


  OSC_1_HiLo = 0;
  PW_HiLo_voice_1 = 0;
  noise_bit_voice_1 = 0;
  pulse_bit_voice_1 = 0;
  sawtooth_bit_voice_1 = 0;
  triangle_bit_voice_1 = 0;
  test_bit_voice_1   = 0;
  ring_bit_voice_1   = 0;
  SYNC_bit_voice_1 = 0;
  Gate_bit_1 = 0;
  ADSR_Attack_1 = 0;
  ADSR_Decay_1 = 0;
  ADSR_Sustain_1 = 0;
  ADSR_Release_1 = 0;
  OSC_2_HiLo = 0;
  PW_HiLo_voice_2 = 0;
  noise_bit_voice_2 = 0;
  pulse_bit_voice_2 = 0;
  sawtooth_bit_voice_2 = 0;
  triangle_bit_voice_2 = 0;
  test_bit_voice_2   = 0;
  ring_bit_voice_2 = 0;
  SYNC_bit_voice_2 = 0;
  Gate_bit_2 = 0;
  ADSR_Attack_2 = 0;
  ADSR_Decay_2 = 0;
  ADSR_Sustain_2 = 0;
  ADSR_Release_2 = 0;
  OSC_3_HiLo = 0;
  PW_HiLo_voice_3 = 0;
  noise_bit_voice_3 = 0;
  pulse_bit_voice_3 = 0;
  sawtooth_bit_voice_3 = 0;
  triangle_bit_voice_3 = 0;
  test_bit_voice_3  = 0;
  ring_bit_voice_3 = 0;
  SYNC_bit_voice_3 = 0;
  Gate_bit_3 = 0;
  ADSR_Attack_3 = 0;
  ADSR_Decay_3 = 0;
  ADSR_Sustain_3 = 0;
  ADSR_Release_3 = 0;
  FILTER_HiLo = 0;
  FILTER_Resonance = 0;
  FILTER_Enable_1 =  0;
  FILTER_Enable_2 = 0;
  FILTER_Enable_3 = 0;
  FILTER_Enable_EXT = 0;
  FILTER_Enable_switch = 0;
  OFF3 =  0;
  FILTER_HP =  0;
  FILTER_BP =  0;
  FILTER_LP =  0;
  MASTER_VOLUME =   0;

}
