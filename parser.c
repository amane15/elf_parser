#include "parser.h"
#include "elf_types.h"

void init_parser(Parser *parser) {
  parser->file = NULL;
  parser->endianess = LittleEndian;
}
