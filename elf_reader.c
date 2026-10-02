#include "elf_reader.h"
#include "elf_types.h"
#include <stdint.h>

uint16_t read_u16(const unsigned char *buf, ELFDataEnc endianess) {
  return endianess == LittleEndian ? (uint16_t)(buf[0] | buf[1] << 8)
                                   : (uint16_t)(buf[0] << 8 | buf[1]);
}

uint32_t read_u32(const unsigned char *buf, ELFDataEnc endianess) {
  return endianess == LittleEndian
             ? (uint32_t)(buf[0] | buf[1] << 8 | buf[2] << 16) |
                   (uint32_t)(buf[3] << 24)
             : (uint32_t)((uint32_t)(buf[0] << 24) | buf[1] << 16 |
                          buf[2] << 8 | buf[3]);
}

uint64_t read_u64(const unsigned char *buf, ELFDataEnc endianess) {
  uint64_t data = 0;
  if (endianess == LittleEndian) {
    for (int i = 7; i >= 0; i--) {
      data = (data << 8) | buf[i];
    }
  } else {
    for (int i = 0; i < 8; i++) {
      data = (data << 8) | buf[i];
    }
  }

  return data;
}
