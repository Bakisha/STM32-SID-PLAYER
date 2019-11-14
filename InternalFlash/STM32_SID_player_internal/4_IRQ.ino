

// IRQ time: see at the bottom of this file

// main SID magic. Every voice is calculated
void irq_handler(HardwareTimer*) {

  PB13_HIGH; // speed test pin // testpoint 1
  //digitalWrite(PB13, HIGH);

  // IRQ time can vary, better to set PWM on previous calculated volume, so at least it's changing at fixed time


  // STM32duino boards
  // Timer1.pause(); // need to pause timer to be able to set value inside irq. Not needed when using ( Timer1.setCompare(TIMER_CH1, main_volume); )
  // TIMER1->CCR1 =  main_volume; //  faster version of Timer1.setCompare(TIMER_CH1, main_volume);
  // Timer1.resume(); // 0.875uS

  // Timer1.setCompare(TIMER_CH1, main_volume); // 0.584 uS


  // STM32 boards
  // analogWrite(PA9, main_volume);
  //PWM->setCaptureCompare(1, main_volume, TICK_COMPARE_FORMAT); // scaled to only 8 bit, i'm tired of fighting against timers and stm32 board core "logic". period*clock must be less then 256
  
  TIM1->CCR1 =  main_volume; //  faster version of PWM->setCaptureCompare(1, main_volume, TICK_COMPARE_FORMAT);

  //PB13_LOW; // test pont

  /*
    skip_counter--; // 1Mhz to 985250HZ difference. Is it needed?
    if (skip_counter == 0) {
    //  PB13_LOW;
    skip_counter = skip_counter_max;
    return;
    }
  */

  VIC_irq = VIC_irq + multiplier; // counting microseconds
  if (VIC_irq >= SID_speed) { // Fake VIC-II irq request (every 20ms ), used in combination with JSR1003 variable from emulator. TODO: read from .sid file at what speed music is played, and change this value accordingly
    VIC_irq_request = 1; // volatile variable for main program
    VIC_irq = 0;
  }

  tune_play_counter = tune_play_counter + multiplier; // counting uS
  if (tune_play_counter >= tune_play_next) { // play next tune
    tune_play_counter = 0;
    if (SID_current_tune == SID_number_of_tunes) {
      SID_current_tune = 1;
    }
    else {
      SID_current_tune++;
    }

    // Init6502; // for now do whole setup routine
    reset6502();
    // resetSID(); // fill SID[] with 0. Untill code is written, hopefully sid init (6502 code of it) routine will handle it
    RAM[0x0304] = SID_current_tune - 1 ; // set song number (-1 offset since in sid header tunes are 1 indexed, in player it's 0 indexed)
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Magic
  ///////////////////////////////////



  OSC_MSB_Previous_1 = OSC_MSB_1;
  OSC_MSB_Previous_2 = OSC_MSB_2;
  OSC_MSB_Previous_3 = OSC_MSB_3;


  OSC_1 = ((!test_bit_voice_1) & 1) * ((OSC_1 + (  multiplier * OSC_1_HiLo)) ) & 0xffffff;
  OSC_2 = ((!test_bit_voice_2) & 1) * ((OSC_2 + (  multiplier * OSC_2_HiLo)) ) & 0xffffff;
  OSC_3 = ((!test_bit_voice_3) & 1) * ((OSC_3 + (  multiplier * OSC_3_HiLo)) ) & 0xffffff;


  // noise_1
  OSC_noise_1 = OSC_noise_1 + multiplier * OSC_1_HiLo; // noise counter (
  OSC_bit19_1 = OSC_noise_1 >> 19 ; //  / 0x080000;// calculate how many missing rising edges of bit_19 since last irq (if any)
  for (i = 0; i < OSC_bit19_1; i++) {
    bit_0_1 = (( bitRead(pseudorandom_1, 22)   ) ^ ((bitRead(pseudorandom_1, 17 ) ) )  ) & 0x1;
    pseudorandom_1 = pseudorandom_1 << 1;
    //pseudorandom_1 = pseudorandom_1 & 0x7fffff;
    pseudorandom_1 = bit_0_1 | pseudorandom_1;
  }
  OSC_noise_1 = OSC_noise_1 - (OSC_bit19_1 << 19); // * 0x080000); // no reset, keep lower 18bit


  // noise_2
  OSC_noise_2 = OSC_noise_2 + multiplier * OSC_2_HiLo; // noise counter (
  OSC_bit19_2 = OSC_noise_2 >> 19 ; // / 0x080000;// calculate how many missing rising edges of bit_19 since last irq
  for (i = 0; i < OSC_bit19_2; i++) {
    bit_0_2 = (( bitRead(pseudorandom_2, 22)   ) ^ ((bitRead(pseudorandom_2, 17 ) ) )  ) & 0x1;
    pseudorandom_2 = pseudorandom_2 << 1;
    //pseudorandom_2 = pseudorandom_2 & 0x7fffff;
    pseudorandom_2 = bit_0_2 | pseudorandom_2;
  }
  OSC_noise_2 = OSC_noise_2 - (OSC_bit19_2 << 19) ; // * 0x080000); // no reset, keep lower 18bits

  // noise_3
  OSC_noise_3 = OSC_noise_3 + multiplier * OSC_3_HiLo; // noise counter (
  OSC_bit19_3 = OSC_noise_3 >> 19 ; // / 0x080000;// calculate how many missing rising edges of bit_19 since last irq
  for (i = 0; i < OSC_bit19_3; i++) {
    bit_0_3 = (( bitRead(pseudorandom_3, 22)   ) ^ ((bitRead(pseudorandom_3, 17 ) ) )  ) & 0x1;
    pseudorandom_3 = pseudorandom_3 << 1;
    //pseudorandom_3 = pseudorandom_3 & 0x7fffff;
    pseudorandom_3 = bit_0_3 | pseudorandom_3;
  }
  OSC_noise_3 = OSC_noise_3 - (OSC_bit19_3 << 19 ); //  * 0x080000); // no reset, keep lower 18bit


  if (OSC_1 >= 0x800000)     OSC_MSB_1 = 1; else OSC_MSB_1 = 0;
  if ( (!OSC_MSB_Previous_1) & (OSC_MSB_1)) MSB_Rising_1 = 1; else  MSB_Rising_1 = 0;

  if (OSC_2 >= 0x800000)     OSC_MSB_2 = 1; else OSC_MSB_2 = 0;
  if ( (!OSC_MSB_Previous_2) & (OSC_MSB_2)) MSB_Rising_2 = 1; else  MSB_Rising_2 = 0;

  if (OSC_3 >= 0x800000)     OSC_MSB_3 = 1; else OSC_MSB_3 = 0;
  if ( (!OSC_MSB_Previous_3) & (OSC_MSB_3)) MSB_Rising_3 = 1; else MSB_Rising_3 = 0;



  if (SYNC_bit_voice_1 & MSB_Rising_3) OSC_1 = OSC_1 & 0x7fffff; // ANDing curent value of OSC with  0x7fffff  i get exact timing when sync happened, no matter of multiplier (and what's more important, what number is in OSC in this exact time)
  if (SYNC_bit_voice_2 & MSB_Rising_1) OSC_2 = OSC_2 & 0x7fffff;
  if (SYNC_bit_voice_3 & MSB_Rising_2) OSC_3 = OSC_3 & 0x7fffff;

  if ( (triangle_bit_voice_1) & (ring_bit_voice_1) ) OSC_MSB_1 = OSC_MSB_1 ^ OSC_MSB_3; // this one took really long time to figure it out. I tought OSC_MSB_1 =  OSC_MSB_3 and everything was wacky with ring modulation
  if ( (triangle_bit_voice_2) & (ring_bit_voice_2) ) OSC_MSB_2 = OSC_MSB_2 ^ OSC_MSB_1; // TODO: see if it's exact on high frequencies
  if ( (triangle_bit_voice_3) & (ring_bit_voice_3) ) OSC_MSB_3 = OSC_MSB_3 ^ OSC_MSB_2; // TODO: see what's faster, here or in triangle voice check



  temp11 = (OSC_1 >> 12); // upper 12 bit of OSC_1 calculate once now

  switch (waveform_switch_1) {
    case 0:
      WaveformDA_1 = 0;
      break;
    case 1:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_1 = WaveformDA_triangle_1;
      break;
    case 2:
      WaveformDA_sawtooth_1 = temp11; // same as upper 12 bits of OSC
      WaveformDA_1 = WaveformDA_sawtooth_1;
      break;
    case 3:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_1 = temp11; // same as upper 12 bits of OSC
      WaveformDA_1 = AND_mask[(WaveformDA_triangle_1 & WaveformDA_sawtooth_1)] << 4; // combined waveform. AND-ed value is take from array (array is actually combined waveform of sawtooth and pulse of 0 value (maximum DC) )
      break;
    case 4:
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_1 = WaveformDA_pulse_1;
      break;
    case 5:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_1 = AND_mask[WaveformDA_triangle_1 & WaveformDA_pulse_1] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_1 = temp11; // same as upper 12 bits of OSC
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_1 = AND_mask[WaveformDA_sawtooth_1 & WaveformDA_pulse_1] << 4;
      break;
    case 7:
      WaveformDA_triangle_1 = ((  (OSC_MSB_1 * B2047) ^ (temp11 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_1 = temp11;
      if (temp11 >= PW_HiLo_voice_1 )  WaveformDA_pulse_1 = B4095; else WaveformDA_pulse_1 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
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


  // end of voice 1

  // PB13_HIGH;


  // voice 2

  temp12 = (OSC_2 >> 12); // upper 12 bit of OSC_2 calculate once now

  switch (waveform_switch_2) {
    case 0:
      WaveformDA_2 = 0;
      break;
    case 1:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_2 = WaveformDA_triangle_2;
      break;
    case 2:
      WaveformDA_sawtooth_2 = temp12;
      WaveformDA_2 = WaveformDA_sawtooth_2;
      break;
    case 3:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_2 = temp12;
      WaveformDA_2 = AND_mask[(WaveformDA_triangle_2 & WaveformDA_sawtooth_2)] << 4;
      break;
    case 4:
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_2 = WaveformDA_pulse_2;
      break;
    case 5:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_2 = AND_mask[WaveformDA_triangle_2 & WaveformDA_pulse_2] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_2 = temp12;
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_2 = AND_mask[WaveformDA_sawtooth_2 & WaveformDA_pulse_2] << 4;
      break;
    case 7:
      WaveformDA_triangle_2 = ((  (OSC_MSB_2 * B2047) ^ (temp12 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_2 = temp12;
      if (temp12 >= PW_HiLo_voice_2 )  WaveformDA_pulse_2 = B4095; else WaveformDA_pulse_2 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
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



  // end of voice 2


  temp13 = (OSC_3 >> 12); // upper 12 bit of OSC_3 calculate once now

  switch (waveform_switch_3) {
    case 0:
      WaveformDA_3 = 0;
      break;
    case 1:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_3 = WaveformDA_triangle_3;
      break;
    case 2:
      WaveformDA_sawtooth_3 = temp13;
      WaveformDA_3 = WaveformDA_sawtooth_3;
      break;
    case 3:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_3 = temp13;
      WaveformDA_3 = AND_mask[(WaveformDA_triangle_3 & WaveformDA_sawtooth_3)] << 4;
      break;
    case 4:
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_3 = WaveformDA_pulse_3;
      break;
    case 5:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_3 = AND_mask[WaveformDA_triangle_3 & WaveformDA_pulse_3] << 4;
      break;
    case 6:
      WaveformDA_sawtooth_3 = temp13;
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
      WaveformDA_3 = AND_mask[WaveformDA_sawtooth_3 & WaveformDA_pulse_3] << 4;
      break;
    case 7:
      WaveformDA_triangle_3 = ((  (OSC_MSB_3 * B2047) ^ (temp13 & B2047)) << 1) ; // (2047 or 0) xor (remaining 11 bits) and left-shifted
      WaveformDA_sawtooth_3 = temp13;
      if (temp13 >= PW_HiLo_voice_3 )  WaveformDA_pulse_3 = B4095; else WaveformDA_pulse_3 = 0;// if upper 12bits oscilator1 is greater then value in d401/d402, then it's zero volume, else it's full
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


  // end of voice 3

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // gate change check
  //
  //  Gate_bit_voice_X variable: for now, main program can set it to any value , irq will reset it to 0
  //   0 - change from 1 to 0 * start Release stage
  //   1 - change from 0 to 1 * start Attack stage


  //


  switch (Gate_bit_1) {
    case 0: // change from 1 to 0 * start Release stage

      if (Gate_previous_1 == 1) {

        ADSR_stage_1 = 4;// Set Release
        LFSR15_1 = 0; // preventing ADSR bugs
        LFSR5_1 = 0; // preventing ADSR bugs
        LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1];
        Gate_previous_1 = 0; // set to 0

        //PB13_HIGH;
      }



      break;
    case 1: // change from 0 to 1 * start Attack/Decay stage
      if (Gate_previous_1 == 0) {

        ADSR_stage_1 = 1; //
        LFSR15_1 = 0;
        LFSR5_1 = 0;
        Gate_previous_1 = 1; // set to 1
        // Switching to attack state unlocks the zero freeze.
        hold_zero_1 = false;
        LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Attack_1];
      }




      //     PB13_LOW;
      break;

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  // Increase LFSR15 counter for ADSR (scaled to match)


  //LFSR15_1 = LFSR15_1 & 0x7fff; // 15bit
  LFSR15_1 = LFSR15_1 + multiplier;;
  // LFSR15_1 = LFSR15_1 & 0x7fff; // 15bit

  if (   ((LFSR15_1 >= LFSR15_comparator_value_1 ) ) ) { //

    Divided_LFSR15_1 = ((LFSR15_1 ) / LFSR15_comparator_value_1); //  count how many time (LFSR15_1 == LFSR15_comparator_value_1 ) was skipped (it's unsigned integer, so no decimals
    LFSR15_1 = LFSR15_1 - Divided_LFSR15_1 * LFSR15_comparator_value_1; // set to zero, plus how manu uS has passed since (LFSR15_1 == LFSR15_comparator_value_1 )
    // LFSR15_1 = 0;
    //  LFSR5_1 = LFSR5_1++;

    //   PB13_HIGH;

    // LFSR5_1 = LFSR5_1 + 1;
    LFSR5_1 = LFSR5_1 + Divided_LFSR15_1 ; // increase LFSR5 counter and check how many (LFSR5_1 == LFSR5_comparator_value_1) was skipped

    if ((ADSR_stage_1 == 1) | (LFSR5_1 >= LFSR5_comparator_value_1) ) {

      //  LFSR5_1 = 0;
      Divided_LFSR5_1 = (LFSR5_1 ) / LFSR5_comparator_value_1; //      ceo5 = (lfsr5 - previous5) / comp5 // comp5 in attack is always 1

      if (Divided_LFSR5_1 >= 1) {
        LFSR5_1 = 0;
      }
      else {
        LFSR5_1 =  LFSR5_1 - Divided_LFSR5_1 * LFSR5_comparator_value_1; //      set to zero, plus how manu uS has passed since (LFSR5_1 == LFSR5_comparator_value_1)
      }

      //     if (hold_zero_1) {
      //       return;
      //    }

      if (hold_zero_1 == false) {
        // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
        switch (ADSR_stage_1) {

          case 0: // release finished, no change

            // lalalalala

            break;
          case 1: // Attack stage
            //  ADSR_volume_1 = (ADSR_volume_1 + 1) & 0xff;
            ADSR_volume_1 = (ADSR_volume_1 + Divided_LFSR15_1) ; // increase  volume value .


            if (ADSR_volume_1 >= 0xff) { // Attack finished, start Decay //
              // Due to time passed since last volume increase, this can be bigger then 0xff.
              // That means that decay stage already started and all values above 0xff is values that should be decrease in decay stage.
              // This is not perfect, those values should be decrementing at decay rate, not as attack rate
              // But is acceptable, because it would be inaccurate only for ADSR_attack values of 0 and 1, where LFSR15_comparator_value is smaller then multiplier.
              // So, timing of decay is off by maximum of <multiplier-1> uS.
              // Other values are decremented at steady -1 rate or less
              ADSR_volume_1 = 0xff - (ADSR_volume_1 - 0xff); // we are in decay stage already, calculate it's volume, depending of uS passed since (ADSR_volume_1 == 0xff)
              ADSR_stage_1 = 2; // set decay stage
              hold_zero_1 = false;
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ]; // set counter comparator to match decay value

            }

            break;
          case 2: // Decay stage

            // 1 step of volume down
            //ADSR_volume_1 = ADSR_volume_1 - 1;
            if (ADSR_volume_1 >= Divided_LFSR5_1)        {
              ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1; //  keep it as positive number
            }
            else {
              ADSR_volume_1 = 0; // no negative numbers, clip it to zero
            }

            if (ADSR_volume_1 <= (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {
              ADSR_volume_1 = (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1); // no matter what number of exact volume is passed, set it to exact value
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Release_1   ]; // set counter comparator to match release value
              // decay finished, time to sustain
              ADSR_stage_1 = 3;

            }
            break;
          case 3: // Sustain stage - checking for change in sustain value
            if (ADSR_volume_1 > (( ADSR_Sustain_1 << 4) + ADSR_Sustain_1)) {

              // new sustain value is smaller then old,  get back to decay.

              ADSR_stage_1 = 2; // set Decay to new value
              LFSR15_comparator_value_1 = ADSR_LFSR15[ADSR_Decay_1   ]; // set counter comparator to match decay value
            }
            break;
          case 4: // Release stage

            //  ADSR_volume_1 = (ADSR_volume_1 - 1) & 0xff;
            if (ADSR_volume_1 >= Divided_LFSR5_1)        {
              ADSR_volume_1 = ADSR_volume_1 - Divided_LFSR5_1; //  keep it as positive number
            }
            else {
              ADSR_volume_1 = 0; // no negative numbers, clip it to zero
            }
            break;

        } // ADSR_stage switch


        LFSR5_comparator_value_1 = ADSR_Volume2LFSR5[ADSR_volume_1]; // must look into table, exact values are not possible


        if (ADSR_volume_1 == 0) {
          hold_zero_1 = true;
        }

      } // not hold zero
    }  // LFSR5_comparator_value check

    //   PB13_LOW;


  }  // LFSR15_comparator_value check

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////
  //ADSR2
  // test point //


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // gate change check
  //
  //  Gate_bit_voice_X variable: for now, main program can set it to any value , irq will reset it to 0
  //   0 - change from 1 to 0 * start Release stage
  //   1 - change from 0 to 1 * start Attack stage


  //


  switch (Gate_bit_2) {
    case 0: // change from 1 to 0 * start Release stage

      if (Gate_previous_2 == 1) {
        ADSR_stage_2 = 4;// Set Release
        LFSR15_2 = 0;
        LFSR5_2 = 0;
        LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2];
        Gate_previous_2 = 0; // set to 0


      }



      break;
    case 1: // change from 0 to 1 * start Attack/Decay stage
      if (Gate_previous_2 == 0) {
        ADSR_stage_2 = 1; //
        LFSR15_2 = 0;
        LFSR5_2 = 0;
        Gate_previous_2 = 1; // set to 1
        // Switching to attack state unlocks the zero freeze.
        hold_zero_2 = false;
        LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Attack_2];
      }


      break;

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // Increase LFSR15 counter for ADSR (scaled to match)



  LFSR15_2 = LFSR15_2 + multiplier;;
  // LFSR15_2 = LFSR15_2 & 0x7fff; // 15bit



  if (   ((LFSR15_2 >= LFSR15_comparator_value_2 ) ) ) { //

    Divided_LFSR15_2 = ((LFSR15_2 ) / LFSR15_comparator_value_2); //  count how many time (LFSR15_2 == LFSR15_comparator_value_2 ) was skipped (it's unsigned integer, so no decimals
    LFSR15_2 = LFSR15_2 - Divided_LFSR15_2 * LFSR15_comparator_value_2; // set to zero, plus how manu uS has passed since (LFSR15_2 == LFSR15_comparator_value_2 )
    // LFSR15_2 = 0;
    //  LFSR5_2 = LFSR5_2++;

    //   PB13_HIGH;

    // LFSR5_2 = LFSR5_2 + 1;
    LFSR5_2 = LFSR5_2 + Divided_LFSR15_2 ; // increase LFSR5 counter and

    if ((ADSR_stage_2 == 1) | (LFSR5_2 >= LFSR5_comparator_value_2) ) {

      //  LFSR5_2 = 0;
      Divided_LFSR5_2 = (LFSR5_2 ) / LFSR5_comparator_value_2; //      check how many (LFSR5_2 == LFSR5_comparator_value_2) was skipped

      if (Divided_LFSR5_2 >= 1) {
        LFSR5_2 = 0;
      }

      if (hold_zero_2 == false) {
        // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
        switch (ADSR_stage_2) {

          case 0: // release finished, no change
            // lalalalala
            break;
          case 1: // Attack stage
            //  ADSR_volume_2 = (ADSR_volume_2 + 1) & 0xff;
            ADSR_volume_2 = (ADSR_volume_2 + Divided_LFSR15_2) ; // increase  volume value (value is unsigned 8bit, but variable is 16bit unsigned integer ), incliding skipped steps ;

            if (ADSR_volume_2 >= 0xff) { // Attack finished, start Decay //
              ADSR_volume_2 = 0xff - (ADSR_volume_2 - 0xff); // we are in decay stage already, calculate it's volume, depending of uS passed since (ADSR_volume_2 == 0xff)
              ADSR_stage_2 = 2; // set decay stage
              hold_zero_2 = false;
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ]; // set counter comparator to match decay value

            }

            break;
          case 2: // Decay stage

            // 1 step of volume down
            //ADSR_volume_2 = ADSR_volume_2 - 1;
            if (ADSR_volume_2 >= Divided_LFSR5_2)        {
              ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2; //  keep it as positive number
            }
            else {
              ADSR_volume_2 = 0; // no negative numbers, clip it to zero
            }

            if (ADSR_volume_2 <= (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
              ADSR_volume_2 = (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2);
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Release_2   ]; // set counter comparator to match release value
              // decay finished, time to sustain
              ADSR_stage_2 = 3;

            }
            break;
          case 3: // Sustain stage - checking for change in sustain value
            if (ADSR_volume_2 > (( ADSR_Sustain_2 << 4) + ADSR_Sustain_2)) {
              // new sustain value is smaller then old,  get back to decay.

              ADSR_stage_2 = 2; // set Decay to new value
              LFSR15_comparator_value_2 = ADSR_LFSR15[ADSR_Decay_2   ]; // set counter comparator to match decay value
            }
            break;
          case 4: // Release stage

            //  ADSR_volume_2 = (ADSR_volume_2 - 1) & 0xff;
            if (ADSR_volume_2 >= Divided_LFSR5_2)        {
              ADSR_volume_2 = ADSR_volume_2 - Divided_LFSR5_2; //  keep it as positive number
            }
            else {
              ADSR_volume_2 = 0; // no negative numbers, clip it to zero
            }
            break;

        } // ADSR_stage switch


        LFSR5_comparator_value_2 = ADSR_Volume2LFSR5[ADSR_volume_2]; // must look into 8bit table, exact values are not possible


        if (ADSR_volume_2 == 0) {
          hold_zero_2 = true;
        }

      } // not hold zero
    }  // LFSR5_comparator_value check

    //   PB13_LOW;


  }  // LFSR15_comparator_value check


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////
  //ADSR3


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // gate change check
  //
  //  Gate_bit_voice_X variable: for now, main program can set it to any value , irq will reset it to 0
  //   0 - change from 1 to 0 * start Release stage
  //   1 - change from 0 to 1 * start Attack stage
  //    2 - no change

  //


  switch (Gate_bit_3) {
    case 0: // change from 1 to 0 * start Release stage

      if (Gate_previous_3 == 1) {
        ADSR_stage_3 = 4;// Set Release
        LFSR15_3 = 0;
        LFSR5_3 = 0;
        LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3];
        Gate_previous_3 = 0; // set to 0

      }



      break;
    case 1: // change from 0 to 1 * start Attack/Decay stage
      if (Gate_previous_3 == 0) {
        ADSR_stage_3 = 1; //
        LFSR15_3 = 0;
        LFSR5_3 = 0;
        Gate_previous_3 = 1; // set to 1
        // Switching to attack state unlocks the zero freeze.
        hold_zero_3 = false;
        LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Attack_3];
      }


      break;

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  // Increase LFSR15 counter for ADSR (scaled to match)



  LFSR15_3 = LFSR15_3 + multiplier;;
  // LFSR15_3 = LFSR15_3 & 0x7fff; // 15bit



  if (   ((LFSR15_3 >= LFSR15_comparator_value_3 ) ) ) { //

    Divided_LFSR15_3 = ((LFSR15_3 ) / LFSR15_comparator_value_3); //  count how many time (LFSR15_3 == LFSR15_comparator_value_3 ) was skipped (it's unsigned integer, so no decimals
    LFSR15_3 = LFSR15_3 - Divided_LFSR15_3 * LFSR15_comparator_value_3; // set to zero, plus how manu uS has passed since (LFSR15_3 == LFSR15_comparator_value_3 )
    // LFSR15_3 = 0;
    //  LFSR5_3 = LFSR5_3++;

    //   PB13_HIGH;

    // LFSR5_3 = LFSR5_3 + 1;
    LFSR5_3 = LFSR5_3 + Divided_LFSR15_3 ; // increase LFSR5 counter and check how many (LFSR5_3 >= LFSR5_comparator_value_3) was skipped

    if ((ADSR_stage_3 == 1) | (LFSR5_3 >= LFSR5_comparator_value_3) ) {

      //  LFSR5_3 = 0;
      Divided_LFSR5_3 = (LFSR5_3 ) / LFSR5_comparator_value_3; //      ceo5 = (lfsr5 - previous5) / comp5 // comp5 in attack is always 1
      if (Divided_LFSR5_3 >= 1) {
        LFSR5_3 = 0;
      }
      else {

        LFSR5_3 =  LFSR5_3 - Divided_LFSR5_3 * LFSR5_comparator_value_3; //      set to zero, plus how manu uS has passed since (LFSR5_3 == LFSR5_comparator_value_3)
      }



      if (hold_zero_3 == false) {
        // 0-release finished , 1-Attack, 2-Decay, 3-Sustain, 4-Release
        switch (ADSR_stage_3) {
          case 0: // release finished, no change
            // lalalalala
            break;
          case 1: // Attack stage
            //  ADSR_volume_3 = (ADSR_volume_3 + 1) & 0xff;
            ADSR_volume_3 = (ADSR_volume_3 + Divided_LFSR15_3) ; // increase  volume value (value is unsigned 8bit, but variable is 16bit unsigned integer ), including skipped steps ;
            if (ADSR_volume_3 >= 0xff) { // Attack finished, start Decay //
              ADSR_volume_3 = 0xff - (ADSR_volume_3 - 0xff); // we are in decay stage already, calculate it's volume, depending of uS passed since (ADSR_volume_3 == 0xff)
              ADSR_stage_3 = 2; // set decay stage
              hold_zero_3 = false;
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ]; // set counter comparator to match decay value
            }
            break;
          case 2: // Decay stage

            // 1 step of volume down
            //ADSR_volume_3 = ADSR_volume_3 - 1;
            if (ADSR_volume_3 >= Divided_LFSR5_3)        {
              ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3; //  keep it as positive number
            }
            else {
              ADSR_volume_3 = 0; // no negative numbers, clip it to zero
            }

            if (ADSR_volume_3 <= (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
              ADSR_volume_3 = (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3);
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Release_3   ]; // set counter comparator to match release value
              // decay finished, time to sustain
              ADSR_stage_3 = 3;

            }
            break;
          case 3: // Sustain stage - checking for change in sustain value
            if (ADSR_volume_3 > (( ADSR_Sustain_3 << 4) + ADSR_Sustain_3)) {
              // new sustain value is smaller then old,  get back to decay.


              ADSR_stage_3 = 2; // set Decay to new value
              LFSR15_comparator_value_3 = ADSR_LFSR15[ADSR_Decay_3   ]; // set counter comparator to match decay value
            }
            break;
          case 4: // Release stage

            //  ADSR_volume_3 = (ADSR_volume_3 - 1) & 0xff;
            if (ADSR_volume_3 >= Divided_LFSR5_3)        {
              ADSR_volume_3 = ADSR_volume_3 - Divided_LFSR5_3; //  keep it as positive number
            }
            else {
              ADSR_volume_3 = 0; // no negative numbers, clip it to zero
            }
            break;

        } // ADSR_stage switch


        LFSR5_comparator_value_3 = ADSR_Volume2LFSR5[ADSR_volume_3]; // must look into table, exact values are not possible


        if (ADSR_volume_3 == 0) {
          hold_zero_3 = true;
        }

      } // not hold zero
    }  // LFSR5_comparator_value check

    //   PB13_LOW;


  }  // LFSR15_comparator_value check




  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //  PB13_HIGH;




  //
  //   FILTERS:
  // Unfortunatelly, i had no idea how filters works, so i just mess with reSID code untill it works out. Far from perfect, but it's good enough...


#ifndef USE_FILTERS // If not using filters for Bluepill's output, invert WaveformDA in case of HP and LP (BP is not inverted)
  // must be done while values are still positive
  if (FILTER_Enable_1) {
    if (FILTER_LP) {
      WaveformDA_1 = 0xfff - WaveformDA_1  ; // MDAC (12bit) inverted
    }
    if (FILTER_HP) {
      WaveformDA_1 = 0xfff - WaveformDA_1 ;
    }
  }
  if (FILTER_Enable_2) {
    if (FILTER_LP) {
      WaveformDA_2 = 0xfff - WaveformDA_2 ; // MDAC (12bit) inverted
    }
    if (FILTER_HP) {
      WaveformDA_2 = 0xfff - WaveformDA_2 ; // MDAC (12bit) inverted
    }
  }
  if (FILTER_Enable_3) {
    if (FILTER_LP) {
      WaveformDA_3 = 0xfff - WaveformDA_3 ;  // MDAC (12bit) inverted
    }
    if (FILTER_HP) {
      WaveformDA_3 = 0xfff - WaveformDA_3 ;  // MDAC (12bit) inverted
    }
  }


#endif



  // finished calculations, time to set main volume

  // WaveformDA : 12bit     ( 0 -> 0x0fff  )
  // ADSR_volume : 8bit     ( 0 -> 0x00ff  )
  // Volume_x = 20bit       ( 0 -> 0xfffff )

  //


  // set WaveformDA of undefined channel to 0
#ifndef USE_CHANNEL_1
  WaveformDA_1 = 0;
#endif
#ifndef USE_CHANNEL_2
  WaveformDA_2 = 0;
#endif
#ifndef USE_CHANNEL_3
  WaveformDA_3 = 0;
#endif



  //////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////// ANALOG ///////////////////////////
  //ok, so far all values are positive, time to go analog
  // Volume       - 20bit , but now signed (+/- 0x7ffff )


  Volume_1 = int32_t(WaveformDA_1 * ADSR_volume_1) - 0x80000 ;
  Volume_2 = int32_t(WaveformDA_2 * ADSR_volume_2) - 0x80000 ;
  Volume_3 = int32_t(WaveformDA_3 * ADSR_volume_3) - 0x80000 ;



  ////////////////////////////////////
  ///////////////////////////////////////////////
  /////////////////////////////////////////////////// FILTERS redirect to filtered or unfiltered output

  switch (FILTER_Enable_switch) {
    default:
    case 0x0:
      Volume_filtered = 0;
      if (OFF3 )
      { // voice3 is not muted if passed thrue filter
        Volume_unfiltered = Volume_1 + Volume_2;
      }
      else {
        Volume_unfiltered = Volume_1 + Volume_2 + Volume_3 ;
      }
      break;
    case 0x1:
      Volume_filtered = Volume_1;
      if (OFF3 )
      { // voice3 is not muted if passed thrue filter
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
      { // voice3 is not muted if passed thrue filter
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
      { // voice3 is not muted if passed thrue filter
        Volume_unfiltered = 0 ;
      }
      else
      {
        Volume_unfiltered = Volume_3 ;
      }
      break;
    case 0x4: // voice3 is included, no matter the state of OFF3
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
  Volume_filter_output = Volume_filtered; // in case filters are skipped


#ifdef USE_FILTERS
  //  PB13_LOW;

  Volume_filter_input = int32_t(Volume_filter_input) >> 7; // lower it to 13bit

  // w0 and Q is calculated emulator's address decoder (to save few us here)
  // Maximum delta cycles for the filter to work satisfactorily under current
  // cutoff frequency and resonance constraints is approximately 8.

  delta_t = multiplier;
  delta_t_flt = FILTER_SENSITIVITY;

  while (delta_t) {
    if (delta_t < delta_t_flt) {
      delta_t_flt = delta_t;
    }

    // delta_t is converted to seconds given a 1MHz clock by dividing
    // with 1 000 000. This is done in two operations to avoid integer
    // multiplication overflow.

    //// Calculate filter outputs.
    //// Vhp = Vbp/Q - Vlp - Vi;
    //// dVbp = -w0*Vhp*dt;
    //// dVlp = -w0*Vbp*dt;
    w0_delta_t = (int32_t(w0_ceil_dt * delta_t_flt) >> 6);

    dVbp = (int32_t(w0_delta_t*Vhp) >> 14);
    dVlp = (int32_t(w0_delta_t*Vbp) >> 14);

    Vbp -= dVbp;
    Vlp -= dVlp;
    Vhp = (int32_t(Vbp * (Q_1024_div)) >> 10) - Vlp - Volume_filter_input; // i am not sure is this order is good. Maybe Vhp is calculated first, before Vbp and Vlp?

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
  //  PB13_HIGH;

  Volume_filter_output = (int32_t(Volume_filter_output) << 7); // back to 20 bit

#endif

  // magic_number - 8bit
  // period       - 8bit
  // Volume       - 20bit
  // MASTER_VOLUME- 4bit

  // time to get positive about it


  Volume = ((Volume_filter_output + Volume_unfiltered) >> 2 ) + 0x80000; // keep as 20bit unsigned value, but it's divided by 4, not 3, because resonance

  if (Volume < 0) Volume = 0;
  if (Volume > 0xfffff) Volume = 0xfffff; // remove clipping (resonance sensitivity), just in case..

  // main_volume_32bit = ( magic_number * period * ((Volume+0x80000)&0xfffff) * MASTER_VOLUME) >> 24; // This could be as large as unsigned 40bit number before shifting, break it down into smaller steps to keep it inside 32bit number bounderies
  main_volume_32bit = (Volume ) ; //& 0xfffff ; // 20bit
  main_volume_32bit = (main_volume_32bit * magic_number); // 28bit
  main_volume_32bit = (main_volume_32bit) >> 12; // 28-12 = 16bit
  main_volume_32bit = (main_volume_32bit *  MASTER_VOLUME); //16+4 =20bit
  main_volume_32bit = (main_volume_32bit * period) ; // 20+8=28bit
  main_volume_32bit = (main_volume_32bit ) >> 12; // 28-20 = 16bit
  main_volume = main_volume_32bit + 1; // i forgot why i added this. Maybe for minimum value for CCR1?



  OSC3 =    ((WaveformDA_3 & 0x400000) >> 11) | // OSC3 output for SID register
            ((WaveformDA_3 & 0x100000) >> 10) |
            ((WaveformDA_3 & 0x010000) >> 7) |
            ((WaveformDA_3 & 0x002000) >> 5) |
            ((WaveformDA_3 & 0x000800) >> 4) |
            ((WaveformDA_3 & 0x000080) >> 1) |
            ((WaveformDA_3 & 0x000010) << 1) |
            ((WaveformDA_3 & 0x000004) << 2);
  //
  ENV3 = (Volume_3 >> 4); // value for REG_28
  //
  SID[25] = POTX;
  SID[26] = POTY;
  SID[27] = OSC3;
  SID[28] = ENV3;

  //

  // btw, lot of "i hope" in this code... oh, well... :-)
  // PB13_LOW; // test pin low. Let's see how much time irq is long (and it's without overhead)
  // testpoint 13 // testpoint 14


  /*
    // Keeping this here in case i change my mind and add 6502 emulation inside IRQ
            // keep running 6502 emulation as long it is not outside of 6502's interrupt routine and last instruction was not SID access,
            // keep executing 6502's code as much as interrupt time allow before overlap (Main program is empty loop, and there is no need to leave any CPU time for him)


    if (JSR1003 == 1) { // JSR1003 check

      if (VIC_irq_request == 1) {


        // disable if emulator is updating from SID[] to variables
        // Just make sure SID[] is volatile
        OSC_1_HiLo = ((SID[0] & 0xff) + ( (SID[1] & 0xff)<<8) ); // *0.985250?
        PW_HiLo_voice_1 = SID[2] + (( (SID[3] & 0x0f)<<8 ));
        noise_bit_voice_1 = ( (SID[4] >> 7 ) & 1) ;
        pulse_bit_voice_1 = ( (SID[4] >> 6 ) & 1) ;
        sawtooth_bit_voice_1 = ( (SID[4] >> 5 ) & 1) ;
        triangle_bit_voice_1 = ( (SID[4] >> 4 ) & 1) ;
        test_bit_voice_1   = ( (SID[4] >> 3 ) & 1) ; //
        ring_bit_voice_1   = ( (SID[4] >> 2 ) & 1) ;
        SYNC_bit_voice_1 = ( (SID[4] >> 1 ) & 1) ; //
        Gate_bit_1 = SID[4] & 1;   //
        waveform_switch_1 = 0x0f & ( SID[4] >> 4);
        ADSR_Attack_1 = ( (SID[5] >> 4 ) & 0x0f) ;
        ADSR_Decay_1 = ( (SID[5]  ) & 0x0f) ;
        ADSR_Sustain_1 = ( (SID[6] >> 4 ) & 0x0f) ;
        ADSR_Release_1 = ( (SID[6]  ) & 0x0f);

        OSC_2_HiLo = (SID[7] + (SID[8]<<8)) ; // *0.985
        PW_HiLo_voice_2 = SID[9] + ((SID[10] & 0x0f)<<8);
        noise_bit_voice_2 = ( (SID[11] >> 7 ) & 1) ;
        pulse_bit_voice_2 = ( (SID[11] >> 6 ) & 1) ;
        sawtooth_bit_voice_2 = ( (SID[11] >> 5 ) & 1) ;
        triangle_bit_voice_2 = ( (SID[11] >> 4 ) & 1) ;
        test_bit_voice_2   = ( (SID[11] >> 3 ) & 1) ; //
        ring_bit_voice_2 = ( (SID[11] >> 2 ) & 1) ; //
        SYNC_bit_voice_2 = ( (SID[11] >> 1 ) & 1) ; //
        Gate_bit_2 = SID[11] & 1;   //
        waveform_switch_2 = 0x0f & ( SID[11] >> 4);
        ADSR_Attack_2 = ( (SID[12] >> 4 ) & 0x0f) ;
        ADSR_Decay_2 = ( (SID[12]  ) & 0x0f) ;
        ADSR_Sustain_2 = ( (SID[13] >> 4 ) & 0x0f) ;
        ADSR_Release_2 = ( (SID[13]  ) & 0x0f);

        OSC_3_HiLo = (SID[14] + ( SID[15]<<8)) ; // *0.985
        PW_HiLo_voice_3 = SID[16] + ((SID[17] & 0x0f)<<8);
        noise_bit_voice_3 = ( (SID[18] >> 7 ) & 1) ;
        pulse_bit_voice_3 = ( (SID[18] >> 6 ) & 1) ;
        sawtooth_bit_voice_3 = ( (SID[18] >> 5 ) & 1) ;
        triangle_bit_voice_3 = ( (SID[18] >> 4 ) & 1) ;
        test_bit_voice_3  = ( (SID[18] >> 3 ) & 1) ; //
        ring_bit_voice_3 = ( (SID[18] >> 2 ) & 1) ; //
        SYNC_bit_voice_3 = ( (SID[18] >> 1 ) & 1) ; //
        Gate_bit_3 = SID[18] & 1;   //
        waveform_switch_3 = 0x0f & ( SID[18] >> 4);
        ADSR_Attack_3 = ( (SID[19] >> 4 ) & 0x0f) ;
        ADSR_Decay_3 = ( (SID[19]  ) & 0x0f) ;
        ADSR_Sustain_3 = ( (SID[20] >> 4 ) & 0x0f) ;
        ADSR_Release_3 = ( (SID[20]  ) & 0x0f);

        FILTER_HiLo = (SID[21] & 0x07) + ( SID[22]<<8); // 11bit // TODO
        FILTER_Resonance = ( (SID[23] >> 4 ) & 0x0f) ;; // 4bit // TODO
        FILTER_Enable_1 =  SID[23]  & 1 ; // on/off
        FILTER_Enable_2 = ( (SID[23] >> 1 ) & 1) ;; // on/off
        FILTER_Enable_3 = ( (SID[23] >> 2 ) & 1) ;; // on/off
        FILTER_Enable_EXT = ( (SID[23] >> 3 ) & 1) ;; // on/off
        OFF3 =  ( (SID[24] >> 7 ) & 1) ;; // on/off; //
        FILTER_HP =  ( (SID[24] >> 6 ) & 1) ;; // on/off; //;
        FILTER_BP =  ( (SID[24] >> 5 ) & 1) ;; // on/off; //;
        FILTER_LP =  ( (SID[24] >> 4 ) & 1) ;; // on/off; //;

        MASTER_VOLUME =   (SID[24]  & 0x0f) ;; // on/off; //; // TODO

        JSR1003 = 0;
        VIC_irq_request = 0;
        frame++;
      }

    }
    else {
      PB12_HIGH;
      exec6502();
    }
    PB12_LOW;
  */

  PB13_LOW;
  // digitalWrite(PB13, LOW);

  STAD4XX = 0; // let main program know that his request has been served
}
// end of irq


// copy code bellow for every instruction to be executed in irq
/*
  PB12_HIGH; // if last instruction had SID access , wait for next irq to serve it
  if (JSR1003 == 1) { // JSR1003 check
    if (VIC_irq_request == 1) {
      JSR1003 = 0;
      VIC_irq_request = 0;
    }
    else {
      // tralala
    }
  }
  else {
      if (STAD4XX == 0) {
      exec6502();
    }
    PB12_LOW;
  }
*/


// IRQ times measured at different setting:


// STM32F013C8T6 (some of them are with 128k or flash)
//
//FILTER_FREQUENCY: 8000
//FILTER_SENSITIVITY: 8
//USE_FILTERS: YES
//
//IRQ Time: 18uS (+/- 2uS)
//6502 instruction time: 4-9uS
// Recommended multiplier: 32 (uS)
//
//
//
//
//
//
