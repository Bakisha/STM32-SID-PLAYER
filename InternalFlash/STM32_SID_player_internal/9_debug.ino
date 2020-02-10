//  from SDfat library
extern "C" char* sbrk(int incr);
// free RAM (actually, free stack
inline uint32_t FreeBytes() {
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));
}

// set serial
inline void debugInit () {
  delay(2000);
#ifdef USE_SERIAL
  Serial.begin(115200); //while (!Serial) {    yield();  }
#endif
#ifdef USE_SERIAL1
  Serial1.begin(115200); //while (!Serial1) {    yield();  }
#endif

}
//  Serial.print
inline void debugPrintTXT (char* output) {
#ifdef USE_SERIAL
  Serial.print(output);
#endif

#ifdef USE_SERIAL1
  Serial1.print(output);
#endif
}

//  Serial.println
inline void debugPrintTXTln (char* output) {
#ifdef USE_SERIAL
  Serial.println(output);
#endif
#ifdef USE_SERIAL1
  Serial1.println(output);
#endif
}

//  Serial.print
inline void debugPrintNUMBER (uint32_t output) {
#ifdef USE_SERIAL
  Serial.print(output);
#endif

#ifdef USE_SERIAL1
  Serial1.print(output);
#endif
}

//  Serial.print(nn,HEX)
inline void debugPrintNUMBERHEX (uint32_t output) {
#ifdef USE_SERIAL
  Serial.print("$");
  Serial.print(output, HEX);
#endif

#ifdef USE_SERIAL1
  Serial1.print("$");
  Serial1.print(output, HEX);
#endif
}
