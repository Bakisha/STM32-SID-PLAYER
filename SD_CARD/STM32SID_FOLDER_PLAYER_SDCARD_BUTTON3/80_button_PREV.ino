
// 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed

//Button press                       BUTTON_PREV ( << )                    BUTTON_PLAY ( > / || )         BUTTON_NEXT ( >> )

//        1                          play previous tune                    play / pause toggle            play next tune
//        2                          play previous file                    restart tune                   play next file
//        3                          play previous folder                  restart tune's time            play next folder
//        4                          play first folder from playlist       SID INFO                       play last folder from playlist
//        5                          HELP                                  HELP                           HELP
//     holding                       Channel 1 OFF                         Channel 2 OFF                  Channel 3 OFF





uint32_t BT_time_start_PREV;
uint32_t BT_time_end_PREV;
uint32_t BT_previous_time_PREV;
uint32_t BT_curent_time_PREV;
int16_t state_PREV = HIGH;
int16_t prevstate_PREV = HIGH;
int16_t clickCount_PREV = 0;
int buttonValue_PREV = 0;


uint32_t BT_debounce_time_PREV = 100;
uint32_t BT_short_click_time_PREV = 650;
uint32_t BT_is_pressed_time_PREV = 1500;
uint32_t BT_short_click_lag_time_PREV = 333;

int GetButtonStatus_PREV() {

#ifdef BUTTON_PREV

  state_PREV = digitalRead(BUTTON_PREV);
  BT_curent_time_PREV = millis();

  if (state_PREV != prevstate_PREV) { // debounce
    if ( (BT_curent_time_PREV - BT_previous_time_PREV) > BT_debounce_time_PREV) {
      BT_previous_time_PREV = BT_curent_time_PREV;
    }
    else {
      state_PREV = prevstate_PREV;
    }
  }

  if ( prevstate_PREV == HIGH && state_PREV == HIGH   ) {

    if (  ((BT_curent_time_PREV - BT_time_end_PREV) > BT_short_click_lag_time_PREV )  ) { // button is "sensed" after timing out, so it have little "lag"
      buttonValue_PREV = clickCount_PREV;
      clickCount_PREV = 0; // reset clickCount_PREV if short click timed out
    }
  }

  if ( prevstate_PREV == HIGH  && state_PREV == LOW) { // button just pressed, just memorize time of a event
    BT_time_start_PREV = BT_curent_time_PREV;
    prevstate_PREV = state_PREV;
  }

  if (prevstate_PREV == LOW && state_PREV == LOW   ) {
    prevstate_PREV = state_PREV;
    if (BT_curent_time_PREV > (BT_time_start_PREV + BT_is_pressed_time_PREV)) {
      clickCount_PREV = 0; //
      buttonValue_PREV =  -1; // is_pressed
    }
  }

  if ( prevstate_PREV == LOW  && state_PREV == HIGH ) { // button released
    BT_time_end_PREV = BT_curent_time_PREV;
    prevstate_PREV = state_PREV;
    if (  ((BT_time_end_PREV - BT_time_start_PREV) < BT_short_click_time_PREV)  ) {
      clickCount_PREV++;
      buttonValue_PREV = 0; //                                                            will keep counting clicks
    }

    if (((BT_time_end_PREV - BT_time_start_PREV)  > BT_is_pressed_time_PREV) ) {
      clickCount_PREV = 0;
      buttonValue_PREV = 0; //                                                             it was is_pressed so far, ignore release
    }
  }

#endif // return 0 if there is no button defined

  return buttonValue_PREV;
}

void checkButton_PREV() {

  switch ( GetButtonStatus_PREV() ) {

    case 0:
      USE_CHANNEL_1    = true;

      break;
    case -1:
      // 1s long press and holding - disable channel 1

      USE_CHANNEL_1    = false;

      break;

    case 1:
      // 1 short click - play previous tune

      mode_play_ON = true; // enable it in case it was off
      tune_mode = 1; // previous
      change_tune = true;
      player = false;


      break;

    case 2:
      // 2 clicks - play previous file

      mode_play_ON = true; // enable it in case it was off
      file_mode = 1 ; // prev file mode
      change_file = true;
      player = false;

      break;

    case 3:
      // 3 clicks -
      // play previous folder

      mode_play_ON = true; // enable it in case it was off
      folder_mode = 1 ; // prev folder mode
      change_folder = true;
      player = false;

      break;

    case 4:
      // 4 clicks - play from 1st folder in the playlist

      mode_play_ON = true; // enable it in case it was off
      current_folder = NUMBER_OF_FOLDERS - 1; // it will switch to 1st
      folder_mode = 0 ; // next folder mode
      change_folder = true;
      player = false;
      break;

    case 5:
      // 5 clicks -  show info about sid file
      infoSID();
      break;

    case 6:
      // 6 clicks -
      HELP();
      break;

    case 7:
      // 7 clicks -

      break;
  }

}
