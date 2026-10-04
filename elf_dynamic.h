#ifndef ELF_DYNAMIC_H
#define ELF_DYNAMIC_H

#include "elf_program.h"
#include "elf_types.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
  int64_t d_tag;
  uint64_t d_val;
} ELF64_Dyn;

int parse_dyn_tb(FILE *fp, ELF64ProgramHeader *dyn_header,
                 ELF64_Dyn dyn_entries[], ELFDataEnc endianess);
void print_dyn_tb(ELF64_Dyn dyn_entries[], size_t entries);

#endif
