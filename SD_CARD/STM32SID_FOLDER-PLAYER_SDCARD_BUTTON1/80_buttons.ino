
// 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed
//                                             - 1 short click  - play next tune
//                                             - 2 short clicks - play next file
//                                             - 3 short clicks - play next folder
//                                             - 4 short clicks - show HELP (on any output defined)
//                                             - 5 short clicks - show info about sid file (on any output defined)
//                                             - 6 short clicks - switch FAVORITE/ALL folder playlist
//                                             - 7 short clicks - switch random play ON/OFF
//                                             - button holding - play tune as fast as possible (fast forward)
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

int GetButtonStatus() {

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

void checkButton1() {

  switch ( GetButtonStatus() ) {

    case 0:
      // tralalala
      break;
    case -1:
      // 3s long press and holding - play tune as fast as possible (fast forward)
      VIC_irq_request = 1;
      break;

    case 1:
      // 1 short click - play next tune
      play_next_tune = true;
      tune_play_counter = 0;
      delay(100);
      break;

    case 2:
      // two clicks - play next file
      load_next_file = true;
      play_next_tune = false;
      try_again = true;
      break;

    case 3:
      // 3 clicks - play next folder
      play_next_folder = true;
      load_next_file = true;
      play_next_tune = false;
      try_again = true;
      break;

    case 4:
      // 4 clicks - show HELP
      HELP();
      break;

    case 5:
      // 5 clicks -  show info about sid file
      infoSID();
      break;

    case 6:
      // 6 clicks - switch/reset between FAVORITE/ALL folder playlist.
#ifdef NUMBER_OF_ALL_FOLDERS // if HVSC folder is included
      favorites_finished =   !favorites_finished ;
#endif
      current_folder = 0;
      play_next_folder = true;
      load_next_file = true;
      play_next_tune = false;
      try_again = true;
      break;

    case 7:
      // 7 clicks -  switch between random and alphabetical playlist
      RANDOM_FOLDERS = !RANDOM_FOLDERS;
      infoSID();
      break;
  }
}
