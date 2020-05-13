

#if defined(SPI_RAM) 
// TODO 
#elif defined(PARALLEL_SRAM) 
// TODO
#else
// internal RAM

//  from SDfat library
extern "C" char* sbrk(int incr);
// free RAM (actually, free stack
inline uint32_t FreeBytes() {
  char top = 't';
  return (128+(&top - reinterpret_cast<char*>(sbrk(0))));
}



inline void POKE (uint16_t addr , uint8_t bytE ) {

  RAM[addr] = bytE;
}

inline uint8_t PEEK (uint16_t addr ) {
  return RAM[addr];
}

inline void AllocateRAM() {

  RAM_SIZE = (FreeBytes() - 2048); // get free memory of microcontroller and leave something for sd card buffers

  if (!RAM) {

    if (RAM_SIZE>0xFFFF) RAM_SIZE=0xFFFF; // 64KB maximum
    RAM = (uint8_t*) calloc(RAM_SIZE, sizeof(uint8_t));
  }
  else {
    // oops
  }
  
}



#endif
