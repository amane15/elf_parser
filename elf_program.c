#include "elf_program.h"
#include "elf_reader.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

bool parse_pg_header(const unsigned char *buf, ELF64ProgramHeader *p_header,
                     ELFDataEnc endianess) {
  size_t offset = 0;

  p_header->p_type = read_u32(buf + offset, endianess);
  offset += 4;

  p_header->p_flags = read_u32(buf + offset, endianess);
  offset += 4;

  p_header->p_offset = read_u64(buf + offset, endianess);
  offset += 8;

  p_header->p_vaddr = read_u64(buf + offset, endianess);
  offset += 8;

  p_header->p_paddr = read_u64(buf + offset, endianess);
  offset += 8;

  p_header->p_filesz = read_u64(buf + offset, endianess);
  offset += 8;

  p_header->p_memsz = read_u64(buf + offset, endianess);
  offset += 8;

  p_header->p_align = read_u64(buf + offset, endianess);
  offset += 8;

  return true;
}

static void print_p_type(uint32_t p_type) {
  printf("p_type: ");
  switch (p_type) {
  case 0:
    printf("NULL\n");
    break;
  case 1:
    printf("LOAD\n");
    break;
  case 2:
    printf("DYNAMIC\n");
    break;
  case 3:
    printf("INTERP\n");
    break;
  case 4:
    printf("NOTE\n");
    break;
  case 5:
    printf("SHLIB\n");
    break;
  case 6:
    printf("PHDR\n");
    break;
  case 7:
    printf("TLS\n");
    break;

  case PT_GNU_EH_FRAME:
    printf("PT_GNU_EH_FRAME\n");
    break;
  case PT_GNU_STACK:
    printf("PT_GNU_STACK\n");
    break;
  case PT_GNU_RELRO:
    printf("PT_GNU_RELRO\n");
    break;
  case PT_GNU_PROPERTY:
    printf("PT_GNU_PROPERTY\n");
    break;
  case PT_GNU_SFRAME:
    printf("PT_GNU_SFRAME\n");
    break;

  default:
    if (p_type >= PT_LOOS && p_type <= PT_HIOS) {
      printf("OS SPECIFIC\n");
    } else if (p_type >= PT_LOPROC && p_type <= PT_HIPROC) {
      printf("PROCESSOR SPECIFIC\n");
    } else {
      printf("Unknown: 0x%" PRIx32 "\n", p_type);
    }
  }
}

void print_program_header(ELF64ProgramHeader *pg_header) {
  print_p_type(pg_header->p_type);
  printf("p_flags: 0x%" PRIx32 "\n", pg_header->p_flags);
  printf("p_offset: 0x%" PRIx64 "\n", pg_header->p_offset);
  printf("p_vaddr: 0x%" PRIx64 "\n", pg_header->p_vaddr);
  printf("p_paddr: 0x%" PRIx64 "\n", pg_header->p_paddr);
  printf("p_filesz: 0x%" PRIx64 "\n", pg_header->p_filesz);
  printf("p_memsz: 0x%" PRIx64 "\n", pg_header->p_memsz);
  printf("p_align: 0x%" PRIx64 "\n", pg_header->p_align);
}
