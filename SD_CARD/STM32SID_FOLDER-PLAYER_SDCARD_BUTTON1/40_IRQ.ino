void irq_handler(void) { //

  SetAUDIO(); // in 20_hardware.ino


  /*
    skip_counter--; // 1Mhz to 985250HZ difference. Is it needed?
    if (skip_counter == 0) {
    skip_counter = skip_counter_max;
    return;
    }
  */

  VIC_irq = VIC_irq + multiplier;
  if (VIC_irq >= SID_speed) {
    VIC_irq_request = 1;
    VIC_irq = 0;
  }



  tune_play_counter = tune_play_counter + multiplier;
  if (tune_play_counter >= tune_end_counter) {
    play_next_tune = true;
  }

  SID_emulator();


  STAD4XX = 0;
}

inline void SID_emulator() {

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  Magic
  //
  ///////////////////////////////////



  OSC_MSB_Previous_1 = OSC_MSB_1;
  OSC_MSB_Previous_2 = OSC_MSB_2;
  OSC_MSB_Previous_3 = OSC_MSB_3;


  OSC_1 = ((!test_bit_voice_1) & 1) * ((OSC_1 + (  multiplier * OSC_1_HiLo)) ) & 0xffffff;
  OSC_2 = ((!test_bit_voice_2) & 1) * ((OSC_2 + (  multiplier * OSC_2_HiLo)) ) & 0xffffff;
  OSC_3 = ((!test_bit_voice_3) & 1) * ((OSC_3 + (  multiplier * OSC_3_HiLo)) ) & 0xffffff;


  // noise_1
  OSC_noise_1 = OSC_noise_1 + multiplier * OSC_1_HiLo;
  OSC_bit19_1 = OSC_noise_1 >> 19 ;
  for (i = 0; i < OSC_bit19_1; i++) {
    bit_0_1 = (( bitRead(pseudorandom_1, 22)   ) ^ ((bitRead(pseudorandom_1, 17 ) ) )  ) & 0x1;
    pseudorandom_1 = pseudorandom_1 << 1;
    //pseudorandom_1 = pseudorandom_1 & 0x7fffff;
    pseudorandom_1 = bit_0_1 | pseudorandom_1;
  }
  OSC_noise_1 = OSC_noise_1 - (OSC_bit19_1 << 19);


  // noise_2
  OSC_noise_2 = OSC_noise_2 + multiplier * OSC_2_HiLo;
  OSC_bit19_2 = OSC_noise_2 >> 19 ;
  for (i = 0; i < OSC_bit19_2; i++) {
    bit_0_2 = (( bitRead(pseudorandom_2, 22)   ) ^ ((bitRead(pseudorandom_2, 17 ) ) )  ) & 0x1;
    pseudorandom_2 = pseudorandom_2 << 1;
    //pseudorandom_2 = pseudorandom_2 & 0x7fffff;
    pseudorandom_2 = bit_0_2 | pseudorandom_2;
  }
  OSC_noise_2 = OSC_noise_2 - (OSC_bit19_2 << 19) ;

  // noise_3
  OSC_noise_3 = OSC_noise_3 + multiplier * OSC_3_HiLo;
  OSC_bit19_3 = OSC_noise_3 >> 19 ;
  for (i = 0; i < OSC_bit19_3; i++) {
    bit_0_3 = (( bitRead(pseudorandom_3, 22)   ) ^ ((bitRead(pseudorandom_3, 17 ) ) )  ) & 0x1;
    pseudorandom_3 = pseudorandom_3 << 1;
    //pseudorandom_3 = pseudorandom_3 & 0x7fffff;
    pseudorandom_3 = bit_0_3 | pseudorandom_3;
  }
  OSC_noise_3 = OSC_noise_3 - (OSC_bit19_3 << 19 );

  if (OSC_1 >= 0x800000)     OSC_MSB_1 = 1; else OSC_MSB_1 = 0;
  if ( (!OSC_MSB_Previous_1) & (OSC_MSB_1)) MSB_Rising_1 = 1; else  MSB_Rising_1 = 0;

  if (OSC_2 >= 0x800000)     OSC_MSB_2 = 1; else OSC_MSB_2 = 0;
  if ( (!OSC_MSB_Previous_2) & (OSC_MSB_2)) MSB_Rising_2 = 1; else  MSB_Rising_2 = 0;

  if (OSC_3 >= 0x800000)     OSC_MSB_3 = 1; else OSC_MSB_3 = 0;
  if ( (!OSC_MSB_Previous_3) & (OSC_MSB_3)) MSB_Rising_3 = 1; else MSB_Rising_3 = 0;



  if (SYNC_bit_voice_1 & MSB_Rising_3) OSC_1 = OSC_1 & 0x7fffff;
  if (SYNC_bit_voice_2 & MSB_Rising_1) OSC_2 = OSC_2 & 0x7fffff;
  if (SYNC_bit_voice_3 & MSB_Rising_2) OSC_3 = OSC_3 & 0x7fffff;

  if ( (triangle_bit_voice_1) & (ring_bit_voice_1) ) OSC_MSB_1 = OSC_MSB_1 ^ OSC_MSB_3;
  if ( (triangle_bit_voice_2) & (ring_bit_voice_2) ) OSC_MSB_2 = OSC_MSB_2 ^ OSC_MSB_1;
  if ( (triangle_bit_voice_3) & (ring_bit_voice_3) ) OSC_MSB_3 = OSC_MSB_3 ^ OSC_MSB_2;

  waveform_switch_1 = (noise_bit_voice_1 << 3) | (pulse_bit_voice_1 << 2) | (sawtooth_bit_voice_1 << 1) | (triangle_bit_voice_1);
  waveform_switch_2 = (noise_bit_voice_2 << 3) | (pulse_bit_voice_2 << 2) | (sawtooth_bit_voice_2 << 1) | (triangle_bit_voice_2);
  waveform_switch_3 = (noise_bit_voice_3 << 3) | (pulse_bit_voice_3 << 2) | (sawtooth_bit_voice_3 << 1) | (triangle_bit_voice_3);

  temp11 = (OSC_1 >> 12);

  switch (waveform_switch_1) {
    case 0:
      WaveformDA_1 = 0;
      break;
    case 1:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
      WaveformDA_1 = WaveformDA_triangle_1;
      break;
    case 2:
      WaveformDA_sawtooth_1 = temp11;
      WaveformDA_1 = WaveformDA_sawtooth_1;
      break;
    case 3:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
      WaveformDA_sawtooth_1 = temp11;
      WaveformDA_1 = AND_mask[(WaveformDA_triangle_1 & WaveformDA_sawtooth_1)] << 4;
      break;
    case 4:
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
      WaveformDA_1 = WaveformDA_pulse_1;
      break;
    case 5:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
      WaveformDA_1 = AND_mask[WaveformDA_triangle_1 & WaveformDA_pulse_1] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_1 = temp11; // same as upper 12 bits of OSC
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
      WaveformDA_1 = AND_mask[WaveformDA_sawtooth_1 & WaveformDA_pulse_1] << 4;
      break;
    case 7:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ;
      WaveformDA_sawtooth_1 = temp11;
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;
      WaveformDA_1 = AND_mask[WaveformDA_pulse_1 & WaveformDA_sawtooth_1 & WaveformDA_triangle_1] << 4;
      break;
    case 8:
      WaveformDA_noise_1 = B4095 & (pseudorandom_1 >> 11);

      WaveformDA_1 =  WaveformDA_noise_1;
      break;
    case 9:
      WaveformDA_1 = 0;
      break;
    case 10:
      WaveformDA_1 = 0;
      break;
    case 11:
      WaveformDA_1 = 0;
      break;
    case 12:
      WaveformDA_1 = 0;
      break;
    case 13:
      WaveformDA_1 = 0;
      break;
    case 14:
      WaveformDA_1 = 0;
      break;
    case 15:
      WaveformDA_1 = 0;
      break;

  }

  // voice 2

  temp12 = (OSC_2 >> 12);

  switch (waveform_switch_2) {
    case 0:
      WaveformDA_2 = 0;
      break;
    case 1:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
      WaveformDA_2 = WaveformDA_triangle_2;
      break;
    case 2:
      WaveformDA_sawtooth_2 = temp12;
      WaveformDA_2 = WaveformDA_sawtooth_2;
      break;
    case 3:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
      WaveformDA_sawtooth_2 = temp12;
      WaveformDA_2 = AND_mask[(WaveformDA_triangle_2 & WaveformDA_sawtooth_2)] << 4;
      break;
    case 4:
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
      WaveformDA_2 = WaveformDA_pulse_2;
      break;
    case 5:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
      WaveformDA_2 = AND_mask[WaveformDA_triangle_2 & WaveformDA_pulse_2] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_2 = temp12;
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
      WaveformDA_2 = AND_mask[WaveformDA_sawtooth_2 & WaveformDA_pulse_2] << 4;
      break;
    case 7:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ;
      WaveformDA_sawtooth_2 = temp12;
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;
      WaveformDA_2 = AND_mask[WaveformDA_pulse_2 & WaveformDA_sawtooth_2 & WaveformDA_triangle_2] << 4;
      break;
    case 8:
      WaveformDA_noise_2 = B4095 & (pseudorandom_2 >> 11);
      WaveformDA_2 =  WaveformDA_noise_2;
      break;
    case 9:
      WaveformDA_2 = 0;
      break;
    case 10:
      WaveformDA_2 = 0;
      break;
    case 11:
      WaveformDA_2 = 0;
      break;
    case 12:
      WaveformDA_2 = 0;
      break;
    case 13:
      WaveformDA_2 = 0;
      break;
    case 14:
      WaveformDA_2 = 0;
      break;
    case 15:
      WaveformDA_2 = 0;
      break;

  }

  // voice 3

  temp13 = (OSC_3 >> 12);

  switch (waveform_switch_3) {
    case 0:
      WaveformDA_3 = 0;
      break;
    case 1:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
      WaveformDA_3 = WaveformDA_triangle_3;
      break;
    case 2:
      WaveformDA_sawtooth_3 = temp13;
      WaveformDA_3 = WaveformDA_sawtooth_3;
      break;
    case 3:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
      WaveformDA_sawtooth_3 = temp13;
      WaveformDA_3 = AND_mask[(WaveformDA_triangle_3 & WaveformDA_sawtooth_3)] << 4;
      break;
    case 4:
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
      WaveformDA_3 = WaveformDA_pulse_3;
      break;
    case 5:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
      WaveformDA_3 = AND_mask[WaveformDA_triangle_3 & WaveformDA_pulse_3] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_3 = temp13;
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
      WaveformDA_3 = AND_mask[WaveformDA_sawtooth_3 & WaveformDA_pulse_3] << 4;
      break;
    case 7:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ;
      WaveformDA_sawtooth_3 = temp13;
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;
      WaveformDA_3 = AND_mask[WaveformDA_pulse_3 & WaveformDA_sawtooth_3 & WaveformDA_triangle_3] << 4;
      break;
    case 8:
      WaveformDA_noise_3 = B4095 & (pseudorandom_3 >> 11);
      WaveformDA_3 =  WaveformDA_noise_3;
      break;
    case 9:
      WaveformDA_3 = 0;
      break;
    case 10:
      WaveformDA_3 = 0;
      break;
    case 11:
      WaveformDA_3 = 0;
      break;
    case 12:
      WaveformDA_3 = 0;
      break;
    case 13:
      WaveformDA_3 = 0;
      break;
    case 14:
      WaveformDA_3 = 0;
      break;
    case 15:
      WaveformDA_3 = 0;
      break;

  }

  //ADSR1

  // gate change check

  switch (Gate_bit_1) {
    case 0:

      if (Gate_previous_1 == 1) {

        ADSR_stage_1 = 4;
        LFSR15_1 = 0;
        LFSR5_1 = 0;
        LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1];
        Gate_previous_1 = 0;
      }
      break;
    case 1:
      if (Gate_previous_1 == 0) {

        ADSR_stage_1 = 1; //
        LFSR15_1 = 0;
        LFSR5_1 = 0;
        Gate_previous_1 = 1;
        hold_zero_1 = false;
        LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Attack_1];
      }
      break;
  }

  // Increase LFSR15 counter for ADSR (scaled to match)

  LFSR15_1 = LFSR15_1 + multiplier;;
  if (   ((LFSR15_1 >= LFSR15_comparator_value_1 ) ) ) {

    Divided_LFSR15_1 = ((LFSR15_1 ) / LFSR15_comparator_value_1);
    LFSR15_1 = LFSR15_1 - Divided_LFSR15_1 * LFSR15_comparator_value_1;
    LFSR5_1 = LFSR5_1 + Divided_LFSR15_1 ;
    if ((ADSR_stage_1 == 1) | (LFSR5_1 >= LFSR5_comparator_value_1) ) {
      Divided_LFSR5_1 = (LFSR5_1 ) / LFSR5_comparator_value_1;
      if (Divided_LFSR5_1 >= 1) {
        LFSR5_1 = 0;
      }
      else {
        LFSR5_1 =  LFSR5_1 - Divided_LFSR5_1 * LFSR5_comparator_value_1;
      }

      if (hold_zero_1 == false) {
        switch (ADSR_stage_1) {
          case 0:
            break;
          case 1:
            ADSR_volume_1 = (ADSR_volume_1 + Divided_LFSR15_1) ;
            if (ADSR_volume_1 >= 0xff) {
              ADSR_volume_1 = 0xff - (ADSR_volume_1 - 0xff);
              ADSR_stage_1 = 2;
              hold_zero_1 = false;
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ];
            }

            break;
          case 2:
            if (ADSR_volume_1 >= Divided_LFSR5_1)        {
              ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1;
            }
            else {
              ADSR_volume_1 = 0;
            }

            if (ADSR_volume_1 <= (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {
              ADSR_volume_1 = (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1);
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1   ];
              ADSR_stage_1 = 3;
            }
            break;
          case 3:
            if (ADSR_volume_1 > (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {
              ADSR_stage_1 = 2;
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ];
            }
            break;
          case 4:
            if (ADSR_volume_1 >= Divided_LFSR5_1)        {
              ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1;
            }
            else {
              ADSR_volume_1 = 0;
            }
            break;

        }

        LFSR5_comparator_value_1 = ADSR_Volume2LFSR5[ADSR_volume_1];


        if (ADSR_volume_1 == 0) {
          hold_zero_1 = true;
        }

      }
    }




  }

  //ADSR2

  // gate change check

  switch (Gate_bit_2) {
    case 0:

      if (Gate_previous_2 == 1) {
        ADSR_stage_2 = 4;
        LFSR15_2 = 0;
        LFSR5_2 = 0;
        LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2];
        Gate_previous_2 = 0;


      }



      break;
    case 1:
      if (Gate_previous_2 == 0) {
        ADSR_stage_2 = 1;
        LFSR15_2 = 0;
        LFSR5_2 = 0;
        Gate_previous_2 = 1;
        hold_zero_2 = false;
        LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Attack_2];
      }
      break;
  }

  // Increase LFSR15 counter for ADSR (scaled to match)

  LFSR15_2 = LFSR15_2 + multiplier;;
  if (   ((LFSR15_2 >= LFSR15_comparator_value_2 ) ) ) {
    Divided_LFSR15_2 = ((LFSR15_2 ) / LFSR15_comparator_value_2);
    LFSR15_2 = LFSR15_2 - Divided_LFSR15_2 * LFSR15_comparator_value_2;
    LFSR5_2 = LFSR5_2 + Divided_LFSR15_2 ;
    if ((ADSR_stage_2 == 1) | (LFSR5_2 >= LFSR5_comparator_value_2) ) {
      Divided_LFSR5_2 = (LFSR5_2 ) / LFSR5_comparator_value_2;
      if (Divided_LFSR5_2 >= 1) {
        LFSR5_2 = 0;
      }
      if (hold_zero_2 == false) {
        switch (ADSR_stage_2) {

          case 0:

            break;
          case 1:

            ADSR_volume_2 = (ADSR_volume_2 + Divided_LFSR15_2) ;

            if (ADSR_volume_2 >= 0xff) {
              ADSR_volume_2 = 0xff - (ADSR_volume_2 - 0xff);
              ADSR_stage_2 = 2;
              hold_zero_2 = false;
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ];
            }
            break;
          case 2:
            if (ADSR_volume_2 >= Divided_LFSR5_2)        {
              ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2;
            }
            else {
              ADSR_volume_2 = 0;
            }
            if (ADSR_volume_2 <= (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
              ADSR_volume_2 = (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2);
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2   ];
              ADSR_stage_2 = 3;
            }
            break;
          case 3:
            if (ADSR_volume_2 > (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
              ADSR_stage_2 = 2;
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ];
            }
            break;
          case 4:
            if (ADSR_volume_2 >= Divided_LFSR5_2)        {
              ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2;
            }
            else {
              ADSR_volume_2 = 0;
            }
            break;
        }

        LFSR5_comparator_value_2 = ADSR_Volume2LFSR5[ADSR_volume_2];

        if (ADSR_volume_2 == 0) {
          hold_zero_2 = true;
        }
      }
    }
  }

  //ADSR3

  // gate change check

  switch (Gate_bit_3) {
    case 0:

      if (Gate_previous_3 == 1) {
        ADSR_stage_3 = 4;
        LFSR15_3 = 0;
        LFSR5_3 = 0;
        LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3];
        Gate_previous_3 = 0;
      }
      break;
    case 1:
      if (Gate_previous_3 == 0) {
        ADSR_stage_3 = 1; //
        LFSR15_3 = 0;
        LFSR5_3 = 0;
        Gate_previous_3 = 1;
        hold_zero_3 = false;
        LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Attack_3];
      }
      break;

  }

  // Increase LFSR15 counter for ADSR (scaled to match)

  LFSR15_3 = LFSR15_3 + multiplier;;
  if (   ((LFSR15_3 >= LFSR15_comparator_value_3 ) ) ) {

    Divided_LFSR15_3 = ((LFSR15_3 ) / LFSR15_comparator_value_3);
    LFSR15_3 = LFSR15_3 - Divided_LFSR15_3 * LFSR15_comparator_value_3;
    LFSR5_3 = LFSR5_3 + Divided_LFSR15_3 ;

    if ((ADSR_stage_3 == 1) | (LFSR5_3 >= LFSR5_comparator_value_3) ) {
      Divided_LFSR5_3 = (LFSR5_3 ) / LFSR5_comparator_value_3;
      if (Divided_LFSR5_3 >= 1) {
        LFSR5_3 = 0;
      }
      else {
        LFSR5_3 =  LFSR5_3 - Divided_LFSR5_3 * LFSR5_comparator_value_3;
      }
      if (hold_zero_3 == false) {

        switch (ADSR_stage_3) {
          case 0:
            break;
          case 1:
            ADSR_volume_3 = (ADSR_volume_3 + Divided_LFSR15_3) ;
            if (ADSR_volume_3 >= 0xff) {
              ADSR_volume_3 = 0xff - (ADSR_volume_3 - 0xff);
              ADSR_stage_3 = 2;
              hold_zero_3 = false;
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ];
            }
            break;
          case 2:
            if (ADSR_volume_3 >= Divided_LFSR5_3)        {
              ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3;
            }
            else {
              ADSR_volume_3 = 0;
            }
            if (ADSR_volume_3 <= (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
              ADSR_volume_3 = (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3);
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3   ];
              ADSR_stage_3 = 3;
            }
            break;
          case 3:
            if (ADSR_volume_3 > (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
              ADSR_stage_3 = 2;
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ];
            }
            break;
          case 4:
            if (ADSR_volume_3 >= Divided_LFSR5_3)        {
              ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3;
            }
            else {
              ADSR_volume_3 = 0;
            }
            break;
        }

        LFSR5_comparator_value_3 = ADSR_Volume2LFSR5[ADSR_volume_3];

        if (ADSR_volume_3 == 0) {
          hold_zero_3 = true;
        }

      }
    }
  }

  //   FILTERS:


#ifndef USE_FILTERS
  if (FILTER_Enable_1) {
    if (FILTER_LP) {
      WaveformDA_1 = 0xfff - WaveformDA_1  ;
    }
    if (FILTER_HP) {
      WaveformDA_1 = 0xfff - WaveformDA_1 ;
    }
  }
  if (FILTER_Enable_2) {
    if (FILTER_LP) {
      WaveformDA_2 = 0xfff - WaveformDA_2 ;
    }
    if (FILTER_HP) {
      WaveformDA_2 = 0xfff - WaveformDA_2 ;
    }
  }
  if (FILTER_Enable_3) {
    if (FILTER_LP) {
      WaveformDA_3 = 0xfff - WaveformDA_3 ;
    }
    if (FILTER_HP) {
      WaveformDA_3 = 0xfff - WaveformDA_3 ;
    }
  }

#endif

#ifndef USE_CHANNEL_1
  WaveformDA_1 = 0;
#endif
#ifndef USE_CHANNEL_2
  WaveformDA_2 = 0;
#endif
#ifndef USE_CHANNEL_3
  WaveformDA_3 = 0;
#endif

  /////////////////// ANALOG ///////////////////////////

  Volume_1 = int32_t(WaveformDA_1 * ADSR_volume_1) - 0x80000 ;
  Volume_2 = int32_t(WaveformDA_2 * ADSR_volume_2) - 0x80000 ;
  Volume_3 = int32_t(WaveformDA_3 * ADSR_volume_3) - 0x80000 ;

  // FILTERS redirect to filtered or unfiltered output

  switch (FILTER_Enable_switch) {
    default:
    case 0x0:
      Volume_filtered = 0;
      if (OFF3 )
      {
        Volume_unfiltered = Volume_1 + Volume_2;
      }
      else {
        Volume_unfiltered = Volume_1 + Volume_2 + Volume_3 ;
      }
      break;
    case 0x1:
      Volume_filtered = Volume_1;
      if (OFF3 )
      {
        Volume_unfiltered =  Volume_2;
      }
      else
      {
        Volume_unfiltered = Volume_2 + Volume_3 ;
      }
      break;
    case 0x2:
      Volume_filtered = Volume_2;
      if (OFF3 )
      {
        Volume_unfiltered = Volume_1  ;
      }
      else
      {
        Volume_unfiltered = Volume_1 + Volume_3 ;
      }
      break;
    case 0x3:
      Volume_filtered = Volume_1 + Volume_2;
      if (OFF3 )
      {
        Volume_unfiltered = 0 ;
      }
      else
      {
        Volume_unfiltered = Volume_3 ;
      }
      break;
    case 0x4:
      Volume_filtered = Volume_3;
      Volume_unfiltered = Volume_1 + Volume_2 ;
      break;
    case 0x5:
      Volume_filtered = Volume_1 + Volume_3;
      Volume_unfiltered = Volume_2 ;
      break;
    case 0x6:
      Volume_filtered = Volume_2 + Volume_3;
      Volume_unfiltered = Volume_1 ;
      break;
    case 0x7:
      Volume_filtered = Volume_1 + Volume_2 + Volume_3;
      Volume_unfiltered = 0;
      break;
  }

  Volume_filter_input = Volume_filtered;
  Volume_filter_output = Volume_filtered;


#ifdef USE_FILTERS

  Volume_filter_input = int32_t(Volume_filter_input) >> 7;

  delta_t = multiplier;
  delta_t_flt = FILTER_SENSITIVITY;

  while (delta_t) {
    if (delta_t < delta_t_flt) {
      delta_t_flt = delta_t;
    }

    w0_delta_t = (int32_t(w0_ceil_dt * delta_t_flt) >> 6);
    dVbp = (int32_t(w0_delta_t*Vhp) >> 14);
    dVlp = (int32_t(w0_delta_t*Vbp) >> 14);
    Vbp -= dVbp;
    Vlp -= dVlp;
    Vhp = (int32_t(Vbp * (Q_1024_div)) >> 10) - Vlp - Volume_filter_input;
    delta_t -= delta_t_flt;
  }

  Volume_filter_output = 0;
  if (FILTER_LP) {
    Volume_filter_output = Volume_filter_output + Vlp;
  }
  if (FILTER_HP) {
    Volume_filter_output = Volume_filter_output + Vhp;
  }
  if (FILTER_BP) {
    Volume_filter_output = Volume_filter_output + Vbp;
  }
  //

  Volume_filter_output = (int32_t(Volume_filter_output) << 7);

#endif

  Volume = ((Volume_filter_output + Volume_unfiltered) >> 2 ) + 0x80000;

  if (Volume < 0) Volume = 0;
  if (Volume > 0xfffff) Volume = 0xfffff;

  // main_volume_32bit = ( magic_number * period * ((Volume)&0xfffff) * MASTER_VOLUME) >> 24;
  main_volume_32bit = (Volume ) ;
  main_volume_32bit = (main_volume_32bit * magic_number);
  main_volume_32bit = (main_volume_32bit) >> 12;
  main_volume_32bit = (main_volume_32bit *  MASTER_VOLUME);
  main_volume_32bit = (main_volume_32bit * period) ;
  main_volume_32bit = (main_volume_32bit ) >> 12;
  main_volume = main_volume_32bit + 1;

  OSC3 =  (OSC_3 >> 16) & 0xff; //
  /*
    OSC3 =  (((OSC_3 & 0x400000) >> 11) | // OSC3 output for SID register
             ((OSC_3 & 0x100000) >> 10) |
             ((OSC_3 & 0x010000) >> 7) |
             ((OSC_3 & 0x002000) >> 5) |
             ((OSC_3 & 0x000800) >> 4) |
             ((OSC_3 & 0x000080) >> 1) |
             ((OSC_3 & 0x000010) << 1) |
             ((OSC_3 & 0x000004) << 2) )&0xff;
  */
  //
  ENV3 = (ADSR_volume_3) & 0xff; ; // ((Volume_3 + 0x80000) >> 12) & 0xff;
  //
  SID[25] = POTX;
  SID[26] = POTY;
  SID[27] = OSC3;
  SID[28] = ENV3;



}
