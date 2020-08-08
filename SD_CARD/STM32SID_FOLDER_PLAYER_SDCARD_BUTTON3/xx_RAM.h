//  from SDfat library
extern "C" char* sbrk(int incr);
// free RAM (actually, free stack
inline uint32_t FreeBytes() {
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));
}




#if defined(SPI_RAM)
// TODO
#elif defined(PARALLEL_SRAM)
// TODO
#else
uint16_t RAM_SIZE = 0;
uint8_t * RAM = NULL;
#endif
