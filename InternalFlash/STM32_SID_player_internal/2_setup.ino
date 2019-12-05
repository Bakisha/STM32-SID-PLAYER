//this could be in setup{}
// but for now, i don't won't to make more mess then it is in main file

#ifdef USE_STM32_ST_CORE
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif

inline void Init6502() {

  if ( (RAM_SIZE ) < (SID_end ) ) {

    LOW_RAM = true;
    // hack to enable playing sids by relocating it's init/play address to lower MPU RAM address,(Mainly for STM32F103C)
    // in RAM, player is at 0x300, .sid file data start at 0x400
    // any address between sid_start and sid_start + RAM_size is read/write, addresses above sid_start + RAM_size is read only
  }
  else {
    LOW_RAM = false;
    // sid start, init and play address is in axact positions in RAM
  }
  //
  // Blue6502 init
  SID_play = SID_data[13] + (SID_data[12] * 256) ;// already defined in sid.h, but for future expansion (when changing subtunes, or maybe SD Card reading)
  SID_init = SID_data[11] + (SID_data[10] * 256);
  SID_speed = 20000; // for now 20000, (in uS) see how to calculate this number from sid header


  if (DEFAULT_SONG > 0) { // if it's 0, upon start, play song that is default in sid header, otherwise, play tune specified as DEFAULT_SONG in "tunes.h"
    SID_current_tune = DEFAULT_SONG;
  }
  // copy player from ROM to RAM

  for ( i = 0; i < 256; i++) { // copy player to 0x300
    RAM[i + 0x0300] = MyROM[i] ; //
  }
  RAM[0x0304] = SID_current_tune - 1 ; // set song numbe (-1 offset since in sid header tunes are 1 indexed, in player it's 0 indexed)
  RAM[0x0307] = SID_data[10]; // init song Hi
  RAM[0x0306] = SID_data[11]; // init song Lo
  RAM[0x0310] = SID_data[12]; // play song Hi
  RAM[0x030f] = SID_data[13]; // play song Lo



  // transfer sid to RAM

  for (uint16_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

    if ( LOW_RAM == true) {
      if (i < (RAM_SIZE - 0x0400) ) { //  hack to load only amount of bytes that can fit into RAM from sid file. Hopefully, player routine is the beginning of file, and tune data is in the end
        RAM[0x400 + i] =  SID_data[i + 0x7e] ; // load .sid to RAM , relocated at 0x0400
      }
    }
    if ( LOW_RAM == false)   {
      RAM[SID_start + i] =  SID_data[i + 0x7e] ; // load .sid directly to RAM in it's exact address . // TODO: Size check
    }

  }


  reset6502(); // reset is at 0x0300

}


// Hardware timers initalization
inline void InitHardware() {

  // init irq

  noInterrupts();


#ifdef  USE_STM32duino_CORE
  pinMode(PB13, OUTPUT); // test pin 1
  pinMode(PB12, OUTPUT); // test pin 2
  pinMode(PB14, INPUT_PULLDOWN); // sense pin (fake PHI2 test)


  pinMode (PA8, PWM); //   main volume output pin. at 1uS timer1, there is 72 (<magic_number>) different values of "volume"


  digitalWrite(PB12, HIGH);
  digitalWrite(PB13, HIGH);
  //pwmWrite(PA8, max_step / 2); // 50% pwm for now



  // Timer1 Channel is on PA8 PWM Pin (i know, i looked at bluepill's pinout)
  // Timer1.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE); // no need, it is already set with: pinMode(PA8, PWM);
  Timer1.setPeriod(period); // every 1 microseconds there will be 72 values of pwm (because bluepill is running at 72Mhz). It can be only be bigger then 1, but smaller then <multiplier>.
  // Timer1.setCompare(TIMER_CH1, 1);      //  pwm of timer - no need to set it now, IRQ will deal with it


  // Timer2 for IRQ
  Timer2.setPrescaleFactor(1);
  Timer2.setMode(TIMER_CH2, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(multiplier); // every xx microseconds interrupt run (total overhead us around 3.4uS +  code runtime (rest is time left for 6502 emulator )
  Timer2.setCompare(TIMER_CH2, 1);      // not needed to put any value, it's irq, it's triggering on overflow, not on compare
  Timer2.attachInterrupt(TIMER_CH2, irq_handler); // i could use better name
#endif




#ifdef USE_STM32_ST_CORE
  pinMode(PA8, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB12, OUTPUT);
  // setup pwm on pin PA8 (timer1,channel1) on <period> us
  // HardwareTimer *PWM = new HardwareTimer(TIM1); // must be before setup{}
  PWM->pause();
  PWM->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA8); // set mode for timer1, channel1 witch correspond to pin PA8 // current mode is set to PWM1. 1 when counter is between zero and CaptureCompare, and 0 when it's between CaptureCompare and overflow value
  PWM->setPrescaleFactor(1);
  PWM->setOverflow( period * magic_number, TICK_FORMAT); //
  PWM->resume();

  // setup irq every <muliplier> us

  // Instantiate HardwareTimer object. Thanks to 'new' instanciation, HardwareTimer is not destructed when setup() function is finished.
  HardwareTimer *IRQtimer = new HardwareTimer(TIM2); // Setup for timer2 (could be any timer, but most of stm32 boards have timers 1 and 2 )
  IRQtimer->setMode(2, TIMER_OUTPUT_COMPARE);  // channel 2, TIMER_OUTPUT_COMPARE is not assotiated on any pin
  IRQtimer->setOverflow(multiplier, MICROSEC_FORMAT); // irq will trigger every <multiplier> uS
  IRQtimer->attachInterrupt(irq_handler); // i could use a mega-super-ultra better name
  IRQtimer->resume();
#endif


  // delay(500);
  interrupts(); // let the show begin
  // end of Blue6502 init
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}
