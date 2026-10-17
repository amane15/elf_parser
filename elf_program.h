#ifndef ELF_PROGRAM_H
#define ELF_PROGRAM_H

#include "elf_types.h"
#include "parser.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PT_TYPE_STR_LEN 32

typedef struct {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} Elf64_Phdr;

typedef struct {
  Elf64_Phdr *entries;
  size_t length;
} Phdr_Table;

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

// Elf64_Phdr *parse_pg_header(Parser *parser);
void parse_pg_header(unsigned char *buf, Elf64_Phdr *phdr,
                     ELFDataEnc endianess);
Phdr_Table *parse_pg_headers(Parser *parser, uint16_t phnum, uint16_t phentsize,
                             uint64_t phoff);

void print_program_header(Phdr_Table *table);
void print_program_header_fmt(Phdr_Table *table);

#endif
