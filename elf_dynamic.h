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

typedef enum {
  DT_NULL = 0,
  DT_NEEDED,
  DT_PLTRELSZ,
  DT_PLTGOT,
  DT_HASH,
  DT_STRTAB,
  DT_SYMTAB,
  DT_RELA,
  DT_RELASZ,
  DT_RELAENT,
  DT_STRSZ,
  DT_SYMENT,
  DT_INIT,
  DT_FINI,
  DT_SONAME,
  DT_RPATH,
  DT_SYMBOLIC,
  DT_REL,
  DT_RELSZ,
  DT_RELENT,
  DT_PLTREL,
  DT_DEBUG,
  DT_TEXTREL,
  DT_JMPREL,
  DT_BIND_NOW,
  DT_LOPROC = 0x70000000,
  DT_HIPROC = 0x7fffffff,
} ELF64_Dtag;

int parse_dyn_tb(FILE *fp, Elf64_Phdr *dyn_header, ELF64_Dyn dyn_entries[],
                 ELFDataEnc endianess);
void print_dyn_tb(ELF64_Dyn dyn_entries[], uint64_t section_offset,
                  int entries);

#endif
