#ifndef ELF_DYNAMIC_H
#define ELF_DYNAMIC_H

#include "elf_program.h"
#include "elf_types.h"
#include "parser.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
  int64_t d_tag;
  uint64_t d_val;
} ELF64_Dyn;

typedef struct {
  ELF64_Dyn *entries;
  uint64_t section_offset;
  size_t length;
} Dyn_Table;

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
  DT_INIT_ARRAY = 0x19,
  DT_FINIARRAY = 0x1a,
  DT_INITARRYSZ = 0x1b,
  DT_FINIARRAYSZ = 0x1c,
  DT_GNU_HASH = 0x6ffffef5,
  DT_FLAGS_1 = 0x6ffffffb,
  DT_VERNEED = 0x6ffffffe,
  DT_VERNEEDNUM = 0x6fffffff,
  DT_VERSYM = 0x6ffffff0,
  DT_RELACOUT = 0x6ffffff9,
  DT_LOPROC = 0x70000000,
  DT_HIPROC = 0x7fffffff,
} ELF64_Dtag;

Dyn_Table *parse_dyn_tb(Parser *parser, Phdr_Table *phdrs_table);
void print_dyn_tb(Dyn_Table *table);
void print_dyn_tb_fmt(Dyn_Table *table);
void free_dyn_table(Dyn_Table *table);

#endif
