#ifndef ELF_READER_H
#define ELF_READER_H

#include "elf_types.h"
#include <stdint.h>

uint16_t read_u16(unsigned char *buf, ELFDataEnc endianess);
uint32_t read_u32(unsigned char *buf, ELFDataEnc endianess);
uint64_t read_u64(unsigned char *buf, ELFDataEnc endianess);

#endif
