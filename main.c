#include "elf_header.h"
#include "elf_program.h"
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

  ELF64ProgramHeader p_headers[rest.e_phnum];
  unsigned char ph_buf[rest.e_phentsize];

  fseek(file, rest.e_phoff, SEEK_SET);

  for (int i = 0; i < rest.e_phnum; i++) {
    if (fread(ph_buf, 1, rest.e_phentsize, file) != rest.e_phentsize) {
      fprintf(stderr, "failed to read program header\n");
      return 1;
    }
    parse_pg_header(ph_buf, &p_headers[i], elf_ident_header.ei_data);
  }

  printf("Program headers: \n");
  // for (int i = 0; i < rest.e_phnum; i++) {
  // printf("=== Program header %d start ===\n", i + 1);
  print_program_header_fmt(p_headers, rest.e_phnum, file);
  // printf("=== Program header %d end ===\n", i + 1);
  // }

  fclose(file);
  return 0;
}
