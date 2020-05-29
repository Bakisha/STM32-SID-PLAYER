
// 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed

//Button press                       BUTTON_PREV ( << )                    BUTTON_PLAY ( > / || )         BUTTON_NEXT ( >> )

//        1                          play previous tune                    play / pause toggle            play next tune
//        2                          play previous file                    restart tune                   play next file
//        3                          play previous folder                  restart tune's time            play next folder
//        4                          play first folder from playlist       SID INFO                       play last folder from playlist
//        5                          HELP                                  HELP                           HELP
//     holding                       Channel 1 OFF                         Channel 2 OFF                  Channel 3 OFF





uint32_t BT_time_start_NEXT;
uint32_t BT_time_end_NEXT;
uint32_t BT_previous_time_NEXT;
uint32_t BT_curent_time_NEXT;
int16_t state_NEXT = HIGH;
int16_t prevstate_NEXT = HIGH;
int16_t clickCount_NEXT = 0;
int buttonValue_NEXT = 0;


uint32_t BT_debounce_time_NEXT = 100;
uint32_t BT_short_click_time_NEXT = 650;
uint32_t BT_is_pressed_time_NEXT = 1500;
uint32_t BT_short_click_lag_time_NEXT = 333;

int GetButtonStatus_NEXT() {

#ifdef BUTTON_NEXT

  state_NEXT = digitalRead(BUTTON_NEXT);
  BT_curent_time_NEXT = millis();

  if (state_NEXT != prevstate_NEXT) { // debounce
    if ( (BT_curent_time_NEXT - BT_previous_time_NEXT) > BT_debounce_time_NEXT) {
      BT_previous_time_NEXT = BT_curent_time_NEXT;
    }
    else {
      state_NEXT = prevstate_NEXT;
    }
  }

  if ( prevstate_NEXT == HIGH && state_NEXT == HIGH   ) {

    if (  ((BT_curent_time_NEXT - BT_time_end_NEXT) > BT_short_click_lag_time_NEXT )  ) { // button is "sensed" after timing out, so it have little "lag"
      buttonValue_NEXT = clickCount_NEXT;
      clickCount_NEXT = 0; // reset clickCount_NEXT if short click timed out
    }
  }

  if ( prevstate_NEXT == HIGH  && state_NEXT == LOW) { // button just pressed, just memorize time of a event
    BT_time_start_NEXT = BT_curent_time_NEXT;
    prevstate_NEXT = state_NEXT;
  }

  if (prevstate_NEXT == LOW && state_NEXT == LOW   ) {
    prevstate_NEXT = state_NEXT;
    if (BT_curent_time_NEXT > (BT_time_start_NEXT + BT_is_pressed_time_NEXT)) {
      clickCount_NEXT = 0; //
      buttonValue_NEXT =  -1; // is_pressed
    }
  }

  if ( prevstate_NEXT == LOW  && state_NEXT == HIGH ) { // button released
    BT_time_end_NEXT = BT_curent_time_NEXT;
    prevstate_NEXT = state_NEXT;
    if (  ((BT_time_end_NEXT - BT_time_start_NEXT) < BT_short_click_time_NEXT)  ) {
      clickCount_NEXT++;
      buttonValue_NEXT = 0; //                                                            will keep counting clicks
    }

    if (((BT_time_end_NEXT - BT_time_start_NEXT)  > BT_is_pressed_time_NEXT) ) {
      clickCount_NEXT = 0;
      buttonValue_NEXT = 0; //                                                             it was is_pressed so far, ignore release
    }
  }

#endif // return 0 if there is no button defined

  return buttonValue_NEXT;
}

void checkButton_NEXT() {

  switch ( GetButtonStatus_NEXT() ) {

    case 0:
      USE_CHANNEL_3    = true;

      break;
    case -1:
      // 3s long press and holding - play tune as fast as possible (fast forward)

      USE_CHANNEL_3    = false;

      break;

    case 1:
      // 1 short click - play next tune
      mode_play_ON = true;
      tune_play_counter = 0;
      change_tune = true;
      tune_mode = 0; // next
      player = false;


      break;

    case 2:
      // two clicks - play next file

      mode_play_ON = true;
      tune_play_counter = 0;
      change_file = true;
      player = false;
      file_mode = 0; // next
      break;

    case 3:
      // 3 clicks - play next folder
      mode_play_ON = true;
      tune_play_counter = 0;
      change_folder = true;
      player = false;
      folder_mode = 0; // next
      break;

    case 4:

      mode_play_ON = true; // enable it in case it was off
      current_folder = NUMBER_OF_FOLDERS - 2; // it will switch to last
      folder_mode = 0 ; // next folder mode
      change_folder = true;
      player = false;
      // 4 clicks - show HELP

      break;

    case 5:
      // 5 clicks -  show info about sid file
      HELP();
      break;

    case 6:
      // 6 clicks -
      infoSID();
      break;

    case 7:
      // 7 clicks -

      break;
  }

}
