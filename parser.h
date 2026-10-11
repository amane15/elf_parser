#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include "elf_types.h"
#include <stdio.h>

typedef struct {
  FILE *file;
  ELFDataEnc endianess;
  ELFClass ei_class;
} Parser;

void init_parser(Parser *parser);

#endif
