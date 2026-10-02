#include "elf_header.h"
#include <stdio.h>

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

  ELFIdent elf_ident_header;

  bool is_parsed = parse_ident_data(file, &elf_ident_header);
  if (!is_parsed) {
    fprintf(stderr, "Error: while parsing idenity header\n");
    fclose(file);
    return 1;
  }

  print_ident_info(&elf_ident_header);

  ELFHeaderRest rest;
  bool is_rest_parsed = parse_rest_header(file, &elf_ident_header, &rest);
  if (!is_rest_parsed) {
    fprintf(stderr, "Error: while parsing rest of the elf header\n");
    fclose(file);
    return 1;
  }

  print_elf_rest_header(&rest);

  fclose(file);
  return 0;
}
