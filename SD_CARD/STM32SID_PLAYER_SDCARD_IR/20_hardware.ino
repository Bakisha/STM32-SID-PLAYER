
// Hardware specifics, for easier porting to other microcontrollers

#ifdef USE_STM32duino_CORE
#ifndef USE_DAC 
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif 
#endif
//
// Set AUDIO_OUT pin PA8 as PWM at <period*F_CPU> overflow, or PA4 as DAC 12bit output 
// Set interrupt at <multiplier> ?S


inline void InitHardware() { // setup pins and IRQ

  // init irq




#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  digitalWrite(LED_BUILTIN, LOW);

#ifdef BUTTON_PREV
  pinMode(BUTTON_PREV, INPUT_PULLUP);
#endif
#ifdef BUTTON_PLAY
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
#endif
#ifdef BUTTON_NEXT
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
#endif


pinMode (irPIN,INPUT_PULLUP);



#ifdef  USE_ROGER_CORE

  pinMode (PA8, PWM); //   audio output pin

  Timer1.setPeriod(period);

  Timer2.setPrescaleFactor(1);
  Timer2.setMode(TIMER_CH2, TIMER_OUTPUT_COMPARE);
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
  PWM->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA8);        // set PA8 as pwm output of Timer1, channel 1
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

inline void SetAUDIO () {

#ifdef USE_ROGER_CORE
  Timer1.setCompare(TIMER_CH1, main_volume);
#endif

#ifdef USE_STM32duino_CORE
 #ifdef USE_DAC
 DAC1->DHR12R1 = main_volume; //  faster version of analogWrite(AUDIO_OUT, main_volume)
 #else // PWM
 TIM1->CCR1 =  main_volume; //  faster version of PWM->setCaptureCompare(1, main_volume, TICK_COMPARE_FORMAT);
#endif // PWM
#endif // CORE

}
