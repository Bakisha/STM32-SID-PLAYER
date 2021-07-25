
// 0=idle, 1,2,3,4...=short clicks,  -1=is_pressed

//Button press                       BUTTON_PREV ( << )                    BUTTON_PLAY ( > / || )         BUTTON_NEXT ( >> )

//        1                          play previous tune                    play / pause toggle            play next tune
//        2                          play previous file                    restart tune                   play next file
//        3                          play previous folder                  restart tune's time            play next folder
//        4                          play first folder from playlist       SID INFO                       play last folder from playlist
//        5                          HELP                                  HELP                           HELP
//     holding                       Channel 1 OFF                         Channel 2 OFF                  Channel 3 OFF





uint32_t BT_time_start_PLAY;
uint32_t BT_time_end_PLAY;
uint32_t BT_previous_time_PLAY;
uint32_t BT_curent_time_PLAY;
int16_t state_PLAY = HIGH;
int16_t prevstate_PLAY = HIGH;
int16_t clickCount_PLAY = 0;
int buttonValue_PLAY = 0;


uint32_t BT_debounce_time_PLAY = 100;
uint32_t BT_short_click_time_PLAY = 650;
uint32_t BT_is_pressed_time_PLAY = 1500;
uint32_t BT_short_click_lag_time_PLAY = 333;

int GetButtonStatus_PLAY() {

#ifdef BUTTON_PLAY

  state_PLAY = digitalRead(BUTTON_PLAY);
  BT_curent_time_PLAY = millis();

  if (state_PLAY != prevstate_PLAY) { // debounce
    if ( (BT_curent_time_PLAY - BT_previous_time_PLAY) > BT_debounce_time_PLAY) {
      BT_previous_time_PLAY = BT_curent_time_PLAY;
    }
    else {
      state_PLAY = prevstate_PLAY;
    }
  }

  if ( prevstate_PLAY == HIGH && state_PLAY == HIGH   ) {

    if (  ((BT_curent_time_PLAY - BT_time_end_PLAY) > BT_short_click_lag_time_PLAY )  ) { // button is "sensed" after timing out, so it have little "lag"
      buttonValue_PLAY = clickCount_PLAY;
      clickCount_PLAY = 0; // reset clickCount_PLAY if short click timed out
    }
  }

  if ( prevstate_PLAY == HIGH  && state_PLAY == LOW) { // button just pressed, just memorize time of a event
    BT_time_start_PLAY = BT_curent_time_PLAY;
    prevstate_PLAY = state_PLAY;
  }

  if (prevstate_PLAY == LOW && state_PLAY == LOW   ) {
    prevstate_PLAY = state_PLAY;
    if (BT_curent_time_PLAY > (BT_time_start_PLAY + BT_is_pressed_time_PLAY)) {
      clickCount_PLAY = 0; //
      buttonValue_PLAY =  -1; // is_pressed
    }
  }

  if ( prevstate_PLAY == LOW  && state_PLAY == HIGH ) { // button released
    BT_time_end_PLAY = BT_curent_time_PLAY;
    prevstate_PLAY = state_PLAY;
    if (  ((BT_time_end_PLAY - BT_time_start_PLAY) < BT_short_click_time_PLAY)  ) {
      clickCount_PLAY++;
      buttonValue_PLAY = 0; //                                                            will keep counting clicks
    }

    if (((BT_time_end_PLAY - BT_time_start_PLAY)  > BT_is_pressed_time_PLAY) ) {
      clickCount_PLAY = 0;
      buttonValue_PLAY = 0; //                                                             it was is_pressed so far, ignore release
    }
  }

#endif // return 0 if there is no button defined

  return buttonValue_PLAY;
}

void checkButton_PLAY() {

  switch ( GetButtonStatus_PLAY() ) {

    case 0:
      USE_CHANNEL_2    = true;

      break;
    case -1:
      // disable channel 2

      USE_CHANNEL_2    = false;

      break;

    case 1:

      mode_play_ON = !mode_play_ON; // play / pause


      // play / pause toggle

      break;

    case 2:

      //    restart tune
      mode_play_ON = true;
      tune_play_counter = 0;
      player = true;

      reset_SID();
      reset6502();



      break;

    case 3:
      // restart tune's time
      mode_play_ON = true;
      tune_play_counter = 0;

      break;

    case 4:
      // SID INFO
      infoSID();
      break;

    case 5:
      // 5 clicks -
      HELP();

      break;

    case 6:
      // 6 clicks -

      break;

    case 7:
      // 7 clicks -

      break;
  }

}
