#include "elf_dynamic.h"
#include "elf_program.h"
#include "elf_reader.h"
#include "elf_types.h"
#include "parser.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Dyn_Table *parse_dyn_tb(Parser *parser, Elf64_Phdr *dyn_header) {
  if (dyn_header->p_type != PT_DYNAMIC) {
    fprintf(stderr, "Non dynamic header is passed");
    return NULL;
  }

  if (fseek(parser->file, (long)dyn_header->p_offset, SEEK_SET) != 0) {
    fprintf(stderr, "Failed to seek PT_DYNAMIC\n");
    return NULL;
  }

  size_t num_of_entries = dyn_header->p_filesz / sizeof(ELF64_Dyn);

  ELF64_Dyn *dyn_entries =
      (ELF64_Dyn *)malloc(sizeof(ELF64_Dyn) * num_of_entries);
  if (dyn_entries == NULL) {
    fprintf(stderr, "Malloc error\n");
    return NULL;
  }

  unsigned char *buf = malloc(dyn_header->p_filesz);
  if (buf == NULL) {
    fprintf(stderr, "Malloc error\n");
    free(dyn_entries);
    return NULL;
  }

  if (fread(buf, 1, dyn_header->p_filesz, parser->file) !=
      dyn_header->p_filesz) {
    fprintf(stderr, "Error while reading dynamic entries");
    free(buf);
    free(dyn_entries);
    return NULL;
  }

  size_t length = 0;
  size_t offset = 0;

  for (size_t i = 0; i < num_of_entries; i++) {
    dyn_entries[i].d_tag = read_u64(buf + offset, parser->endianess);
    offset += 8;
    dyn_entries[i].d_val = read_u64(buf + offset, parser->endianess);
    offset += 8;

    length++;

    if (dyn_entries[i].d_tag == DT_NULL) {
      break;
    }
  }

  Dyn_Table *table = (Dyn_Table *)malloc(sizeof(Dyn_Table));
  if (table == NULL) {
    fprintf(stderr, "Malloc error\n");
    free(buf);
    free(dyn_entries);
    return NULL;
  }

  table->entries = dyn_entries;
  table->length = length;
  table->entries = dyn_entries;

  free(buf);

  return table;
}

void print_dyn_tb(ELF64_Dyn *dyn_entries, uint64_t section_offset,
                  int entries) {
  printf("Dynamic section at the offset %" PRIx64 " contains %d"
         " entries\n",
         section_offset, entries);
  for (int i = 0; i < entries; i++) {
    printf("Tag: %" PRIx64 "\n", dyn_entries[i].d_tag);
    printf("Val: %" PRIx64 "\n", dyn_entries[i].d_val);
  }
}

void free_dyn_table(Dyn_Table *table) {
  if (table == NULL)
    return;

  free(table->entries);
  free(table);
}
