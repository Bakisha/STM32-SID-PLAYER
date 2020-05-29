//  from SDfat library
extern "C" char* sbrk(int incr);
// free RAM (actually, free stack
inline uint32_t FreeBytes() {
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));
}



// set serial
inline void debugInit () {

#ifdef USE_SERIAL
  Serial.begin(SERIAL_SPEED); // while (!Serial) {    yield();  }
#endif
#ifdef USE_SERIAL1
  Serial1.begin(SERIAL_SPEED); //while (!Serial1) {    yield();  }
#endif
  // delay(2000);

}
//  Serial.print
inline void debugPrintTXT (const char* output) {
#ifdef USE_SERIAL
  Serial.print(output);
#endif

#ifdef USE_SERIAL1
  Serial1.print(output);
#endif

}

//  Serial.println
inline void debugPrintTXTln (const char* output) {
#ifdef USE_SERIAL
  Serial.println(output);
#endif
#ifdef USE_SERIAL1
  Serial1.println(output);
#endif


}

//  Serial.print
inline void debugPrintNUMBER (const int32_t output) {
#ifdef USE_SERIAL
  Serial.print(output);
#endif

#ifdef USE_SERIAL1
  Serial1.print(output);
#endif

}

//  Serial.print(nn,HEX)
inline void debugPrintNUMBERHEX (const int32_t output) {
#ifdef USE_SERIAL
  Serial.print("$");
  Serial.print(output, HEX);
#endif

#ifdef USE_SERIAL1
  Serial1.print("$");
  Serial1.print(output, HEX);
#endif

}
