
// 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed
int16_t buttonState;
uint32_t BT_time_start;
uint32_t BT_time_end;
uint32_t BT_previous_time;
uint32_t BT_curent_time;
int16_t state = HIGH;
int16_t prevState = HIGH;
int16_t clickCount = 0;
int buttonValue = 0;


uint32_t BT_debounce_time = 100;
uint32_t BT_short_click_time = 650;
uint32_t BT_is_pressed_time = 1500;
uint32_t BT_short_click_lag_time = 333;

inline int GetButtonStatus() {

  state = digitalRead(BUTTON_1);
  BT_curent_time = millis();

  if (state != prevState) { // debounce
    if ( (BT_curent_time - BT_previous_time) > BT_debounce_time) {
      BT_previous_time = BT_curent_time;
    }
    else {
      state = prevState;
    }
  }

  if ( prevState == HIGH && state == HIGH   ) {

    if (  ((BT_curent_time - BT_time_end) > BT_short_click_lag_time )  ) { // button is "sensed" after timing out, so it have little "lag"
      buttonValue = clickCount;
      clickCount = 0; // reset clickCount if short click timed out
    }
  }

  if ( prevState == HIGH  && state == LOW) { // button just pressed, just memorize time of a event
    BT_time_start = BT_curent_time;
    prevState = state;
  }

  if (prevState == LOW && state == LOW   ) {
    prevState = state;
    if (BT_curent_time > (BT_time_start + BT_is_pressed_time)) {
      clickCount = 0; //
      buttonValue =  -1; // is_pressed
    }
  }


  if ( prevState == LOW  && state == HIGH ) { // button released
    BT_time_end = BT_curent_time;
    prevState = state;
    if (  ((BT_time_end - BT_time_start) < BT_short_click_time)  ) {
      clickCount++;
      buttonValue = 0; //                                                            will keep counting clicks
    }

    if (((BT_time_end - BT_time_start)  > BT_is_pressed_time) ) {
      clickCount = 0;
      buttonValue = 0; //                                                             it was is_pressed so far, ignore release
    }
  }



  return buttonValue;
}
