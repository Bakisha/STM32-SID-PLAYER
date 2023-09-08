

#if defined(SPI_RAM)
// TODO
#elif defined(PARALLEL_SRAM)
// TODO
#else
// internal RAM

void AllocateRAM() {
  if ((FreeBytes()) > 0xffff ) {
    RAM_SIZE = 0xffff;
  }
  else {
    RAM_SIZE = FreeBytes() - 2048;
  }
  RAM = (uint8_t*) calloc(RAM_SIZE+1, sizeof(uint8_t)); // allocate memory
}

inline void POKE (uint16_t addr , uint8_t bytE ) {

  RAM[addr] = bytE;
}

inline uint8_t PEEK (uint16_t addr ) {
  return RAM[addr];
}

#endif
