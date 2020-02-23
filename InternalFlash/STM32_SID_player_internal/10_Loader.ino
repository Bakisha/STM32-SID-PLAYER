
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void header_INTERNAL () {


  for (int header = 0; header < 0x7e; header++) {
        POKE (header + 0x0380, SID_data[header]);
  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void INTERNAL_LOAD() {


  for (uint16_t i = 0; i < SID_data_size - 0x7e; i++) { // data start at $7e offset

    if ( LOW_RAM == true) {
      if (i < (RAM_SIZE - 0x0400) ) { //  hack to load only amount of bytes that can fit into RAM from sid file. Hopefully, player routine is the beginning of file, and tune data is in the end
   POKE (0x400 + i,  SID_data[i + 0x7e]) ;      // load .sid to RAM , relocated at 0x0400
      }
    }
    if ( LOW_RAM == false)   {
     POKE (SID_load_start + i, SID_data[i + 0x7e] );    // load .sid directly to RAM in it's exact address . // TODO: Size check
    }

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline void Loader() {



  header_INTERNAL(); //  load header to RAM at $0380 (so it can also be accessed by 6502 code)
  PLAYABLE_SID = Compatibility_check(); // set sid's globals (from RAM), true if sid is playble

  // if (PLAYABLE_SID) { // disabled for internal version. 
  player_setup();
  INTERNAL_LOAD();
  infoSID(); // print out info on any output that is defined
  reset6502(); // reset CPU
  POKE (0x0304, SID_current_tune - 1) ; // set tune number

  //  }





}
