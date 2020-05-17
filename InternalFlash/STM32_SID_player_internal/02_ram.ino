

#if defined(SPI_RAM) 
// TODO 
#elif defined(PARALLEL_SRAM) 
// TODO
#else
// internal RAM

inline void POKE (uint16_t addr , uint8_t bytE ) {

  RAM[addr] = bytE;
}

inline uint8_t PEEK (uint16_t addr ) {
  return RAM[addr];
}

#endif
