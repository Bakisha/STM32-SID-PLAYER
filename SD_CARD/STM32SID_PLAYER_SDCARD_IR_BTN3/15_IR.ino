// original source code :                     https://github.com/perz/irNEC
// my edited version with timer interrupts :  https://github.com/Bakisha/irNEC_STM32_TIM2_Callback

/*

   Manual (for my remote)

   - PLAY    -    play/pause
             -    <HOLD> SID INFO
   - PREV    -    previous file
   - NEXT    -    next file
   - 0       -    next subtune
                  <HOLD> HELP
   - LOOP    -    previous folder
   - SCAN    -    next folder
   - 1       -    file-50
   - 2       -    file+50
   - 3       -    random file (from the same folder)
   - 4       -    folder-5
   - 5       -    folder+5
   - 6       -    random folder
   - 7       -    restart tune
   - 8       -    extend tune time by 3 minutes
   - 9       -    <HOLD> fast forward (play as as fast as possible)


  +----------------------------+
  |  IR REMOTE                 |
  |                            |
  |----------------------------|
  |   +----+  +----+  +----+   |
  |   |POW |  |MODE|  |MUTE|   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |PLAY|  |PREV|  |NEXT|   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |EQ  |  |VOL-|  |VOL+|   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |0   |  |LOOP|  |SCAN|   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |1   |  |2   |  |3   |   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |4   |  |5   |  |6   |   |
  |   +----+  +----+  +----+   |
  |                            |
  |   +----+  +----+  +----+   |
  |   |7   |  |8   |  |9   |   |
  |   +----+  +----+  +----+   |
  +----------------------------+

*/



volatile unsigned long irCommand;

bool waitForStart = false;
bool startSet = false;
volatile bool gotIr = false;
volatile bool repeatIr = false;
volatile byte irBits, irAdr,  irData;


#define NECREPEAT 8000
#define NECSTARTHIGH  14000
#define NECSTARTLOW 4000
#define NECONE 1688
#define NECZERO 800
#define NECONEHIGH 2812

bool irPIN_state_current;
bool irPIN_state_prev;
uint32_t irTimer_current;
uint32_t irTimer_prev;
uint32_t irTimer_diff;




/*  Read NEC ir interrupt function
  Use: irTmr 4bytes
    irCommand 4 bytes
    irBits   1 byte
    irData 1 byte   Some knob has been pushed :-) on the remote if gotIr is true read it and handle (check if its the right adress maybe) after handled set gotIr to false
    irAdr 1 byte  The remote has this adress
    and four boolean
    gotIr =  a ircommand is waiting to be handled
    repeatIr = a ircommand AND a repeatsignal has been sent
*****************************************/
inline void irRead() {

  // call from interrupt routine every <multiplier> microseconds

  irTimer_current = irTimer_current + multiplier;

  irPIN_state_current = bool(digitalRead(irPIN));


  if ( (irPIN_state_prev == true) & (irPIN_state_current == false) ) { // falling edge

    irTimer_diff = irTimer_current - irTimer_prev;


    if (startSet) {
      if (irTimer_diff > NECONEHIGH) {

        if (irTimer_diff > NECREPEAT)
          repeatIr = true;
        startSet = false;
        irBits = 0;

      }
      else if (irTimer_diff > NECONE) {

        bitSet(irCommand, (31 - irBits));
        irBits++;

      }
      else if (irTimer_diff > NECZERO) {
        bitClear(irCommand, (31 - irBits));
        irBits++;
      }
      else {
        startSet = false;
        irBits = 0;

      }
    }
    else if (irTimer_diff > NECSTARTHIGH) {
      waitForStart = true;

    }
    else if (irTimer_diff > NECSTARTLOW) {
      startSet = true;
      irBits = 0;
    }

    if (irBits == 32) {
      startSet = false;
      irBits = 0;
      gotIr = true;
      irData = (byte)(( irCommand >> 8) & 0xFF);
      //irDataN = (byte)(0xff & irCommand);
      irAdr = (byte)((irCommand >> 24) & 0xFF);


    }

    irTimer_prev = irTimer_current; // remember the time of falling edge
  }
  irPIN_state_prev = irPIN_state_current;


}


////////////////////////////////////////////////////////////////////////////////
//
//  return 0 if no key is pressed
//  any positive value is key code pressed for first time (255 codes possible)
//  any negative values is key code that is still pressed (repeat) (255 codes possible)
//  NOTE: calling this in a range of miliseconds or more (it needs around 70 miliseconds to register key press)
//
////////////////////////////////////////////////////////////////////////////////

inline int32_t IRgetCode () {

  int32_t return_code = 0x00; // if gotIr or repeatIr is false, return 0

  if (gotIr) {

    gotIr = false;
    return_code = irData; // return positive for first press
    // return_code=irAdr;

  }
  if (repeatIr) {
    // return negative value for repeated key
    return_code = -irData; //
    repeatIr = false;
  }


  return return_code;
}


////////////////////////////////////////////////////////////////////////////////
//
//  call IRgetCode and act on it's return value
//
////////////////////////////////////////////////////////////////////////////////

inline void checkIR () {


  switch (IRgetCode()) { // get code from IR
    case 0:
      // tralalalala nothing pressed;

      break;

    case irPLAY:
      // play / pause
      mode_play_ON = !mode_play_ON; // play / pause

      break;

    case irPREV:
      //  play previous file

      mode_play_ON = true; // enable it in case it was off
      file_mode = 1 ; // prev file mode
      change_file = true;
      player = false;
      tune_play_counter = 0;
      break;

    case irNEXT:
      // play next file
      mode_play_ON = true;
      tune_play_counter = 0;
      change_file = true;
      player = false;
      file_mode = 0; // next
      break;

    case irLOOP:
      // play previous folder

      mode_play_ON = true; // enable it in case it was off
      folder_mode = 1 ; // prev folder mode
      change_folder = true;
      player = false;
      tune_play_counter = 0;
      break;

    case irSCAN:
      // play next folder
      mode_play_ON = true;
      tune_play_counter = 0;
      change_folder = true;
      player = false;
      folder_mode = 0; // next
      break;

    case irZERO:
      //  play next tune
      mode_play_ON = true;
      tune_play_counter = 0;
      change_tune = true;
      tune_mode = 0; // next
      player = false;
      break;

    case irONE:
      // file-50

      mode_play_ON = true; // PLAY/PAUSE set to PLAY
      tune_play_counter = 0;
      change_file = true;
      player = false;
      file_mode = 1; // prev
      if (current_file > 50) {
        current_file = current_file - 49; // if it's more then 50, lower by 49 , it will be lowered by 1 again in change_file check (1 indexed)
      }
      else {
        current_file = 2; // will be lowered to 1 on change_file check
      }
      break;

    case irTWO:
      // file+50
      mode_play_ON = true; // PLAY/PAUSE set to PLAY
      tune_play_counter = 0;
      change_file = true;  // changing file mode
      player = false;      // disable SID emulation in interrupt
      file_mode = 0; // next
      current_file = current_file + 49; // it will be added by 1 again in change_file check (with limitation check for total_sid_files)
      break;

    case irTHREE:
      // random file
      mode_play_ON = true; // PLAY/PAUSE set to PLAY
      tune_play_counter = 0;
      change_file = true;  // changing file mode
      player = false;      // disable SID emulation in interrupt
      file_mode = 0; // next
      current_file = random(total_sid_files); // let's see how much random it is
      break;

    case irFOUR:
      // folder-5
      mode_play_ON = true; // enable it in case it was off
      folder_mode = 1 ; // prev folder mode
      change_folder = true;
      player = false;
      tune_play_counter = 0;
      if (current_folder > 4) { // zero indexed
        current_folder = current_folder - 4;// will be lowered by 1 again in change_folder
      }
      else {
        current_folder = 1; // will be lowered by 1 in change_folder (zero indexed)
      }

      break;

    case irFIVE:
      //  folder+5
      mode_play_ON = true;
      tune_play_counter = 0;
      change_folder = true;
      player = false;
      folder_mode = 0; // next
      current_folder = current_folder + 4; // will be raised by 1 again in change_folder
      break;
  
    case irSIX:
      // random folder
      mode_play_ON = true; // enable it in case it was off
      folder_mode = 1 ; // prev folder mode (only way to play folder 0)
      change_folder = true;
      player = false;
      tune_play_counter = 0;
      current_folder = 1 + random(NUMBER_OF_FOLDERS); // will be lowered by, also, give last folder a chanse
      break;

    case irSEVEN:
      // restart tune
      mode_play_ON = true;
      tune_play_counter = 0;
      player = true;

      reset_SID();
      reset6502();

      break;
    case irEIGHT:
      // extend tune as much as defined default play time
      tune_end_counter = tune_end_counter + TUNE_PLAY_TIME;
      break;
    case irNINE:

      break;
    case -irPLAY:
      // SID INFO
      infoSID();

      break;
    case -irPREV:

      break;
    case -irNEXT:

      break;
    case -irLOOP:

      break;
    case -irSCAN:

      break;
    case -irZERO:
      // HELP
      HELP();
      break;
    case -irONE:

      break;
    case -irTWO:

      break;
    case -irTHREE:

      break;
    case -irFOUR:

      break;
    case -irFIVE:

      break;
    case -irSIX:

      break;
    case -irSEVEN:

      break;
    case -irEIGHT:

      break;
    case -irNINE:

      VIC_irq_request = 1;

      break;

  }

}
