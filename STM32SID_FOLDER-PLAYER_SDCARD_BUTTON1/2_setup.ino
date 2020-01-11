#ifdef USE_STM32_ST_CORE
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif

inline void InitHardware() {

  // init irq

  noInterrupts();

  pinMode(BUTTON_1, INPUT_PULLUP);

#ifdef  USE_STM32duino_CORE
  pinMode(PB13, OUTPUT); // test pin 1
  pinMode(PB12, OUTPUT); // test pin 2
  pinMode (PA8, PWM); //   audio output pin

  digitalWrite(PB12, HIGH);
  digitalWrite(PB13, HIGH);

  Timer1.setPeriod(period);

  Timer2.setPrescaleFactor(1);
  Timer2.setMode(TIMER_CH2, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(multiplier);
  Timer2.setCompare(TIMER_CH2, 1);
  Timer2.attachInterrupt(TIMER_CH2, irq_handler);
#endif

#ifdef USE_STM32_ST_CORE
  pinMode(PA8, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB12, OUTPUT);

  PWM->pause();
  PWM->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA8);
  PWM->setPrescaleFactor(1);
  PWM->setOverflow( period * magic_number, TICK_FORMAT);
  PWM->resume();


  HardwareTimer *IRQtimer = new HardwareTimer(TIM2);
  IRQtimer->setMode(2, TIMER_OUTPUT_COMPARE);
  IRQtimer->setOverflow(multiplier, MICROSEC_FORMAT);
  IRQtimer->attachInterrupt(irq_handler);
  IRQtimer->resume();
#endif

  interrupts();

}

inline void Compatibility_check() {

  MagicID = RAM[0 +  0x0380] ;


  VERSION =  RAM[0x05 +  0x0380];

  RAM_OVERFLOW = false;
  if ( ( SID_data_size + 0x0400 - 0x7e) > RAM_SIZE    ) {
    RAM_OVERFLOW = true;
  }
  LOAD_ADDRESS = 0 ;
  SID_load_start = (RAM[0x08 +  0x0380] * 256) + (RAM[0x09 +  0x0380]);

  if (SID_load_start == 0) {
    SID_load_start = RAM[0x7c + 0x380] + (RAM[0x7d + 0x380] * 256);
  }
  if ( SID_load_start >= 0x07E8)  {
    LOAD_ADDRESS = SID_load_start;
  }
  SID_load_end = SID_load_start + SID_data_size - 0x7e ;
  if ( (RAM_SIZE ) < (SID_load_end ) ) {
    LOW_RAM = true;
  }
  else {
    LOW_RAM = false;
  }

  SID_play =  RAM[0x0d + 0x0380] + (RAM[0x0c + 0x0380] * 256);
  SID_init = RAM[0x0b + 0x0380] + (RAM[0x0a + 0x0380] * 256);
  SID_number_of_tunes =  RAM[0x0f + 0x0380] + ( RAM[0x0e + 0x0380] * 256);
  SID_default_tune =  RAM[0x11 + 0x0380] + ( RAM[0x10 + 0x0380] * 256);
  SID_current_tune =  SID_default_tune;
  SID_speed = 20000;

  if (DEFAULT_SONG > 0) {
    SID_current_tune = DEFAULT_SONG;
  }

}

inline void player_setup() {

  for ( i = 0; i < 0x80; i++) {
    RAM[i + 0x0300] = MyROM[i] ; //
  }
  RAM[0x0304] = SID_current_tune - 1 ;
  RAM[0x0307] = (SID_init >> 8) & 0xff;
  RAM[0x0306] = SID_init & 0xff;
  RAM[0x0310] = (SID_play >> 8) & 0xff;
  RAM[0x030f] = SID_play & 0xff;

}

inline void header_SD () {

  sidfile.seekSet( 0 );
  for (int header = 0; header < 0x7e; header++) {
    RAM[header + 0x0380] = sidfile.read();
  }

}

inline void SD_LOAD() {

  sidfile.seekSet(0x7e );
  for (uint16_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

    if ( LOW_RAM == true) {
      if (i < (RAM_SIZE - 0x0400) ) {
        RAM[0x400 + i] =  sidfile.read() ;
      }
    }
    if ( LOW_RAM == false)   {
      RAM[SID_load_start + i] =  sidfile.read() ;
    }
  }

}
