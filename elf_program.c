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

static const char *pt_type(uint32_t p_type) {
  switch (p_type) {
  case 0:
    return "NULL";
  case 1:
    return "LOAD";
  case 2:
    return "DYNAMIC";
  case 3:
    return "INTERP";
  case 4:
    return "NOTE";
  case 5:
    return "SHLIB";
  case 6:
    return "PHDR";
  case 7:
    return "TLS";

  case PT_GNU_EH_FRAME:
    return "PT_GNU_EH_FRAME";
  case PT_GNU_STACK:
    return "PT_GNU_STACK";
  case PT_GNU_RELRO:
    return "PT_GNU_RELRO";
  case PT_GNU_PROPERTY:
    return "PT_GNU_PROPERTY";
  case PT_GNU_SFRAME:
    return "PT_GNU_SFRAME";

  default:
    if (p_type >= PT_LOOS && p_type <= PT_HIOS) {
      return "OS SPECIFIC";
    } else if (p_type >= PT_LOPROC && p_type <= PT_HIPROC) {
      return "PROCESSOR SPECIFIC";
    } else {
      return "Unknown";
    }
  }
}

static void pghdr_flags(uint32_t flags, char out[4]) {
  out[0] = (flags & PF_R) ? 'R' : ' ';
  out[1] = (flags & PF_W) ? 'W' : ' ';
  out[2] = (flags & PF_X) ? 'E' : ' ';
  out[3] = '\0';
}

void print_program_header(ELF64ProgramHeader *pg_header) {
  char flags[4];
  pghdr_flags(pg_header->p_flags, flags);

  printf("p_type: %s\n", pt_type(pg_header->p_type));
  printf("p_flags: %s\n", flags);
  printf("p_offset: 0x%" PRIx64 "\n", pg_header->p_offset);
  printf("p_vaddr: 0x%" PRIx64 "\n", pg_header->p_vaddr);
  printf("p_paddr: 0x%" PRIx64 "\n", pg_header->p_paddr);
  printf("p_filesz: 0x%" PRIx64 "\n", pg_header->p_filesz);
  printf("p_memsz: 0x%" PRIx64 "\n", pg_header->p_memsz);
  printf("p_align: 0x%" PRIx64 "\n", pg_header->p_align);
}

void print_program_header_fmt(ELF64ProgramHeader phdrs[], size_t phnum,
                              FILE *fp) {
  printf("%-16s %-10s %-10s %-10s %-10s %-10s %-4s %-8s \n", "Type", "Offset",
         "VirtAddr", "PhysAddr", "Filesz", "Memsz", "Flag", "Align");

  for (size_t i = 0; i < phnum; i++) {
    ELF64ProgramHeader phdr = phdrs[i];
    const char *type = pt_type(phdr.p_type);
    char flags[4];
    pghdr_flags(phdr.p_flags, flags);

    printf("%-16s 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64
           " 0x%08" PRIx64 " %-3s 0x%" PRIx64 "\n",
           type, phdr.p_offset, phdr.p_vaddr, phdr.p_paddr, phdr.p_filesz,
           phdr.p_memsz, flags, phdr.p_align);

    if (phdr.p_type == PT_INTERP) {
      char buf[phdr.p_filesz + 1];
      fseek(fp, phdr.p_offset, SEEK_SET);
      if (fread(buf, 1, phdr.p_filesz, fp) != phdr.p_filesz) {
        fprintf(stdout, "Error loading interp exec name");
        continue;
      }

      buf[phdr.p_filesz] = '\0';
      printf("    Requesting program interpreter: %s\n", buf);
    }
  }
}
