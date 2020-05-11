
// Hardware specifics, for easier porting to other microcontrollers

#ifdef USE_STM32duino_CORE
HardwareTimer *PWM = new HardwareTimer(TIM1); // need to set it up here, before setup{}
#endif

//
// Set PA8 pin as PWM, at <period*F_CPU> overflow
// Set interrupt at <multiplier>


inline void InitHardware() { // setup pins and IRQ

  // init irq

  noInterrupts();

  pinMode(BUTTON_1, INPUT_PULLUP);

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
  pinMode(PA8, OUTPUT);

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

inline void SetAUDIO () {

#ifdef USE_ROGER_CORE
  Timer1.setCompare(TIMER_CH1, main_volume);
#endif

#ifdef USE_STM32duino_CORE
  TIM1->CCR1 =  main_volume; //  faster version of PWM->setCaptureCompare(1, main_volume, TICK_COMPARE_FORMAT);
#endif

}
