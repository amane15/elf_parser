#ifndef ELF_PROGRAM_H
#define ELF_PROGRAM_H

#include "elf_types.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} ELF64ProgramHeader;

typedef enum {
  PT_NULL = 0,
  PT_LOAD,
  PT_DYNAMIC,
  PT_INTERP,
  PT_NOTE,
  PT_SHLIB,
  PT_PHDR,
  PT_TLS,
  PT_LOOS = 0x60000000,
  PT_GNU_EH_FRAME = 0x6474e550,
  PT_GNU_STACK = 0x6474e551,
  PT_GNU_RELRO = 0x6474e552,
  PT_GNU_PROPERTY = 0x6474e553,
  PT_GNU_SFRAME = 0x6474e554,
  PT_HIOS = 0x6fffffff,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7fffffff,
} PGH_PTYPE;

typedef enum {
  PF_X = 1,
  PF_W = 2,
  PF_R = 4,
} PT_FLAGS;

bool parse_pg_header(const unsigned char *buf, ELF64ProgramHeader *p_header,
                     ELFDataEnc ei_data);

void print_program_header(ELF64ProgramHeader *pg_header);

#endif
