#include "elf_dynamic.h"
#include "elf_header.h"
#include "elf_program.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Error: Missing file argument\n");
    fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (file == NULL) {
    fprintf(stderr, "Error opening file\n");
    return 1;
  }

  Parser parser;
  init_parser(&parser);
  parser.file = file;

  ELFIdent *ident = parse_ident_header(&parser);
  if (ident == NULL) {
    fprintf(stderr, "Error: while parsing idenity header\n");
    fclose(file);
    return 1;
  }
  parser.endianess = ident->ei_data;
  parser.ei_class = ident->ei_class;

  print_ident_info(ident);

  ELFHeader *rest = parse_rest_header(&parser);
  if (rest == NULL) {
    fprintf(stderr, "Error: while parsing rest of the elf header\n");
    fclose(file);
    return 1;
  }

  print_elf_rest_header(rest);

  Phdr_Table *phdrs_table = parse_pg_headers(&parser, rest->e_phnum,
                                             rest->e_phentsize, rest->e_phoff);
  printf("Program headers: \n");
  print_program_header_fmt(phdrs_table);

  Dyn_Table *dyn_table = parse_dyn_tb(&parser, phdrs_table);
  if (dyn_table == NULL) {
    fprintf(stderr, "Error while parsing dynamic table\n");
    return 1;
  }
  print_dyn_tb_fmt(dyn_table);

  free(ident);
  free(rest);

  free_phdr_table(phdrs_table);
  free_dyn_table(dyn_table);

  fclose(file);
  return 0;
}
