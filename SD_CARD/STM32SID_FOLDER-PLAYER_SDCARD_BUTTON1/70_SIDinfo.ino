
char  SIDinfo_filetype [8] ;
char  SIDinfo_name [33] ;
char  SIDinfo_author [33] ;
char  SIDinfo_released [33] ;
char  SIDinfo_VIDEO [14] ;
char  SIDinfo_CLOCK [7] ;
char  SIDinfo_PLAYABLE [6];
char  SIDinfo_MODEL [20];


inline void  infoSID() {

  // - PSID (0x50534944)
  // - RSID (0x52534944)

  strcpy (SIDinfo_filetype, "UNKNOWN"); // if PSID/RSID check fail

  if ( (RAM[0x380 + 00] == 0x50) & (RAM[0x380 + 01] == 0x53) & (RAM[0x380 + 02] == 0x49) & (RAM[0x380 + 03] == 0x44) ) {
    strcpy (SIDinfo_filetype, "PSID");
  }

  if ( (RAM[0x380 + 00] == 0x52) & (RAM[0x380 + 01] == 0x53) & (RAM[0x380 + 02] == 0x49) & (RAM[0x380 + 03] == 0x44) ) {
    strcpy (SIDinfo_filetype, "RSID");
  }

  strcpy (SIDinfo_name, "");
  for (int cc = 0; cc < 0x20; cc = cc + 1) {
    SIDinfo_name[cc] = (RAM[0x380 + 0x16 + cc]);
    if (cc == 0x1f) {
      SIDinfo_name[0x20] = 0; // null terminating string
    }
  }
  strcpy (SIDinfo_author, "");
  for (int cc = 0; cc < 0x20; cc = cc + 1) {
    SIDinfo_author[cc] = (RAM[0x380 + 0x36 + cc]);
    if (cc == 0x1f) {
      SIDinfo_author[0x20] = 0; // null terminating string
    }
  }
  strcpy (SIDinfo_released, "");
  for (int cc = 0; cc < 0x20; cc = cc + 1) {
    SIDinfo_released[cc] = (RAM[0x380 + 0x56 + cc]);
    if (cc == 0x1f) {
      SIDinfo_released[0x20] = 0; // null terminating string
    }
  }


  switch (VIDEO_TYPE) {
    case 0:
      strcpy (SIDinfo_VIDEO, "UNKNOWN");
      break;
    case 1:
      strcpy (SIDinfo_VIDEO, "PAL");
      break;
    case 2:
      strcpy (SIDinfo_VIDEO, "NTSC");
      break;
    case 3:
      strcpy (SIDinfo_VIDEO, "PAL and NTSC");
      break;

  }
  switch (SUBTUNE_SPEED) {
    case 0:
      strcpy (SIDinfo_CLOCK, "VIC II");
      break;
    case 1:
      strcpy (SIDinfo_CLOCK, "CIA");
      break;
  }

  if (PLAYABLE_SID) {
    strcpy (SIDinfo_PLAYABLE, "OK");
  } else {
    strcpy (SIDinfo_PLAYABLE, "ERROR");
  }

  switch (MODEL_TYPE) {
    case 0:
      strcpy (SIDinfo_MODEL, "UNKNOWN");
      break;
    case 1:
      strcpy (SIDinfo_MODEL, "MOS6581");
      break;
    case 2:
      strcpy (SIDinfo_MODEL, "MOS8580");
      break;
    case 3:
      strcpy (SIDinfo_MODEL, "MOS6581 and MOS8580");
      break;
  }

  debugPrintTXTln ("");
  debugPrintTXT   ("Player:    "); debugPrintTXT  (SIDinfo_PLAYABLE); debugPrintTXTln ("");
  debugPrintTXT   ("Size:      "); debugPrintNUMBER(SID_data_size); debugPrintTXT(" bytes"); debugPrintTXTln (" ");
  debugPrintTXT   ("Type:      "); debugPrintTXT (SIDinfo_filetype); debugPrintTXTln ("");
  debugPrintTXT   ("Name:      "); debugPrintTXT (SIDinfo_name); debugPrintTXTln ("");
  debugPrintTXT   ("Author:    "); debugPrintTXT (SIDinfo_author); debugPrintTXTln ("");
  debugPrintTXT   ("Released:  "); debugPrintTXT (SIDinfo_released); debugPrintTXTln ("");
  debugPrintTXT   ("Range:     "); debugPrintNUMBERHEX(SID_load_start); debugPrintTXT(" - "); debugPrintNUMBERHEX(SID_load_end); debugPrintTXTln ("");
  debugPrintTXT   ("Init:      "); debugPrintNUMBERHEX(SID_init); debugPrintTXTln ("");
  debugPrintTXT   ("Play:      "); debugPrintNUMBERHEX(SID_play); debugPrintTXTln ("");
  debugPrintTXT   ("Video:     "); debugPrintTXT   (SIDinfo_VIDEO); debugPrintTXTln ("");
  debugPrintTXT   ("Clock:     "); debugPrintTXT   (SIDinfo_CLOCK); debugPrintTXTln ("");
  debugPrintTXT   ("SID model: "); debugPrintTXT   (SIDinfo_MODEL); debugPrintTXTln ("");

  SID_info_tune(); // subtune print
}

inline void SID_info_tune() { //          this will be called from Sid_info() and when changing subtune
  debugPrintTXT   ("Tune:      ");
  debugPrintNUMBER(SID_current_tune);
  debugPrintTXT   ("/");
  debugPrintNUMBER(SID_number_of_tunes);
  debugPrintTXT   (" (");
  debugPrintNUMBER(SID_default_tune);
  debugPrintTXT   (")") ;
  debugPrintTXTln ("");
}

// calculate free unused RAM
inline void HELP () {
  debugPrintTXTln  ("");
  debugPrintTXT    ("    **** STM32 SID PLAYER ****    ");
  debugPrintTXTln  ("");
  debugPrintTXT    ("  ");
  debugPrintNUMBER ((RAM_SIZE >> 10) + 1);
  debugPrintTXT    ("K RAM SYSTEM  ");
  debugPrintNUMBER (FreeBytes());
  debugPrintTXT    (" BYTES FREE");
  debugPrintTXTln  ("");
  debugPrintTXT    ("  ");
  debugPrintNUMBER (uint32_t(1000000 / multiplier));
  debugPrintTXT    ("Hz Samplerate ");
  debugPrintNUMBER (uint32_t( (1000000 / period) / 1000));
  debugPrintTXTln  ("KHz PWM frequency");
  debugPrintTXTln  ("");

  debugPrintTXTln  ("- 1 short click  - play next tune");
  debugPrintTXTln  ("- 2 short clicks - play next file");
  debugPrintTXTln  ("- 3 short clicks - play next folder");
  debugPrintTXTln  ("- 4 short clicks - show HELP");
  debugPrintTXTln  ("- 5 short clicks - show info about sid file");
  debugPrintTXTln  ("- 6 short clicks - switch FAVORITE/ALL folder list");
  debugPrintTXTln  ("- button holding - play tune as fast as possible (fast forward)");


  debugPrintTXTln  ("");
  debugPrintTXTln  ("READY.");
}
