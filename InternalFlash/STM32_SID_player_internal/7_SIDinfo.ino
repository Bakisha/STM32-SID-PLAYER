
char  SIDinfo_filetype [8] ;
char  SIDinfo_name [33] ;
char  SIDinfo_author [33] ;
char  SIDinfo_released [33] ;


inline void  infoSID() {

  /*
    Data size:        bytes
    File type:
    Name:
    Author:
    Released:
    Load range:      $0900-$214E
    Init address:    $0900
    Play address:    $0903
    Number of songs: 1
    Default song:    1
    Clock speed:     PAL
    SID model:       8580
  */
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

  debugPrintTXTln (" ");
  debugPrintTXT   ("Data size:                           "); debugPrintNUMBER(SID_data_size); debugPrintTXT(" bytes"); debugPrintTXTln (" ");
  debugPrintTXT   ("File type:                           "); debugPrintTXT (SIDinfo_filetype); debugPrintTXTln (" ");
  debugPrintTXT   ("Name:                                "); debugPrintTXT (SIDinfo_name); debugPrintTXTln (" ");
  debugPrintTXT   ("Author:                              "); debugPrintTXT (SIDinfo_author); debugPrintTXTln (" ");
  debugPrintTXT   ("Released:                            "); debugPrintTXT (SIDinfo_released); debugPrintTXTln (" ");
  debugPrintTXT   ("Load range:                          "); debugPrintNUMBERHEX(SID_load_start); debugPrintTXT(" - "); debugPrintNUMBERHEX(SID_load_end); debugPrintTXTln (" ");
  debugPrintTXT   ("Init address:                        "); debugPrintNUMBERHEX(SID_init); debugPrintTXTln (" ");
  debugPrintTXT   ("Play address:                        "); debugPrintNUMBERHEX(SID_play); debugPrintTXTln (" ");
  debugPrintTXT   ("Number of songs:                     "); debugPrintNUMBER(SID_number_of_tunes); debugPrintTXTln (" ");
  debugPrintTXT   ("Default song:                        "); debugPrintNUMBER(SID_default_tune); debugPrintTXTln (" ");
  debugPrintTXT   ("Clock speed:                         "); debugPrintTXT   ("Unimplemented/TODO"); debugPrintTXTln (" ");
  debugPrintTXT   ("SID model:                           "); debugPrintTXT   ("Unimplemented/TODO"); debugPrintTXTln (" ");
  debugPrintTXT   ("Compatibility:                       "); // this info will be finished when sid is loaded/played

}
