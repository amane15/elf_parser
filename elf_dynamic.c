#include "elf_dynamic.h"
#include "elf_program.h"
#include "elf_reader.h"
#include "elf_types.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int parse_dyn_tb(FILE *fp, ELF64ProgramHeader *dyn_header,
                 ELF64_Dyn dyn_entries[], ELFDataEnc endianess) {
  if (dyn_header->p_type != PT_DYNAMIC) {
    fprintf(stderr, "Non dynamic header is passed");
    return -1;
  }

  uint64_t total_dyn_entries = dyn_header->p_filesz / sizeof(ELF64_Dyn);

  printf("Dynamic section at the offset %" PRIx64 " contains %" PRId64
         " entries\n",
         dyn_header->p_offset, total_dyn_entries);

  if (fseek(fp, (long)dyn_header->p_offset, SEEK_SET) != 0) {
    fprintf(stderr, "Failed to seek PT_DYNAMIC\n");
    return -1;
  }

  unsigned char buf[dyn_header->p_filesz];

  if (fread(buf, 1, dyn_header->p_filesz, fp) != dyn_header->p_filesz) {
    fprintf(stderr, "Error while reading dynamic entries");
    return -1;
  }

  size_t offset = 0;
  for (uint64_t i = 0; i < total_dyn_entries; i++) {
    dyn_entries[i].d_tag = read_u64(buf + offset, endianess);
    offset += 8;
    dyn_entries[i].d_val = read_u64(buf + offset, endianess);
    offset += 8;
  }

  return 0;
}

void print_dyn_tb(ELF64_Dyn *dyn_entries, size_t entries) {
  for (size_t i = 0; i < entries; i++) {
    printf("Tag: %" PRIx64 "\n", dyn_entries[i].d_tag);
    printf("Val: %" PRIx64 "\n", dyn_entries[i].d_val);
  }
}
