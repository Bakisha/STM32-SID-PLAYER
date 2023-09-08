// core detector
#ifdef USE_HAL_DRIVER // Official ST cores. Support for multiple line of MPU
#define USE_STM32duino_CORE //  Set in preferences: https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json and search stm32 in board manager. Choose stm32 cores by ST Microelectronics. Select your CPU from tools menu)
#ifdef USE_DAC
#define AUDIO_OUT       PA4           // PA4 is DAC1 output on most STM32 boards
#else
#define AUDIO_OUT       PA8           // PA8 is output of Timer1, channel 1 on most STM32 boards
#endif
#else
#define USE_ROGER_CORE //  Set in preferences: http://dan.drown.org/stm32duino/package_STM32duino_index.json and search stm32F1 in board manager. Choose STM32F1xx core (NOTE: Only STM32F1 works)
#endif


#ifdef USE_STM32duino_CORE
#ifndef USE_DAC 
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif 
#endif




inline void InitHardware() { // setup pins and IRQ

  // init irq

  

#ifdef  USE_ROGER_CORE

  pinMode (PA8, PWM); //   audio output pin

  Timer1.setPeriod(period);

  Timer2.setPrescaleFactor(1);
  Timer2.setMode(TIMER_CH2, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(multiplier);
  Timer2.setCompare(TIMER_CH2, 1);
  Timer2.attachInterrupt(TIMER_CH2, irq_handler);
#endif

#ifdef USE_STM32duino_CORE
 pinMode(AUDIO_OUT, OUTPUT);

#ifdef USE_DAC   // Use DAC on pin PA4

analogWrite(AUDIO_OUT, 0x000);    // Set PA4 as analog output, write once, so next write can be done directly to register

#else     // use PWM
  PWM->pause();
  PWM->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, AUDIO_OUT);        // set PA8 as pwm output of Timer1, channel 1
  PWM->setPrescaleFactor(1);
  PWM->setOverflow( period * magic_number, TICK_FORMAT);  // CPU_MHz*period
  PWM->resume();
#endif

  HardwareTimer *IRQtimer = new HardwareTimer(TIM2);
  
  IRQtimer->setOverflow(multiplier, MICROSEC_FORMAT);     // set callback at samplerate
  IRQtimer->attachInterrupt(irq_handler);
  IRQtimer->resume();
#endif

 

}
