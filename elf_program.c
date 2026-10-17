#include "elf_program.h"
#include "elf_dynamic.h"
#include "elf_reader.h"
#include "elf_types.h"
#include <endian.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void parse_pg_header(unsigned char *buf, Elf64_Phdr *phdr,
                     ELFDataEnc endianess) {
  size_t offset = 0;

  phdr->p_type = read_u32(buf + offset, endianess);
  offset += 4;

  phdr->p_flags = read_u32(buf + offset, endianess);
  offset += 4;

  phdr->p_offset = read_u64(buf + offset, endianess);
  offset += 8;

  phdr->p_vaddr = read_u64(buf + offset, endianess);
  offset += 8;

  phdr->p_paddr = read_u64(buf + offset, endianess);
  offset += 8;

  phdr->p_filesz = read_u64(buf + offset, endianess);
  offset += 8;

  phdr->p_memsz = read_u64(buf + offset, endianess);
  offset += 8;

  phdr->p_align = read_u64(buf + offset, endianess);
  offset += 8;
}

Phdr_Table *parse_pg_headers(Parser *parser, uint16_t phnum, uint16_t phentsize,
                             uint64_t phoff) {
  if (phentsize != sizeof(Elf64_Phdr)) {
    fprintf(stderr, "Unexpected program header size\n");
    return NULL;
  }

  if (phnum == 0) {
    return NULL;
  }

  size_t total_size = phentsize * phnum;

  Elf64_Phdr *phdrs = (Elf64_Phdr *)malloc(sizeof(Elf64_Phdr) * phnum);
  if (phdrs == NULL) {
    return NULL;
  }

  unsigned char *buf = malloc(total_size);
  if (buf == NULL) {
    free(phdrs);
    return NULL;
  }

  if (fseek(parser->file, (long)phoff, SEEK_SET) != 0) {
    fprintf(stderr, "Error while seeking file\n");
    free(phdrs);
    free(buf);
    return NULL;
  }

  if (fread(buf, 1, total_size, parser->file) != total_size) {
    fprintf(stderr, "Error while reading program headers\n");
    free(phdrs);
    free(buf);
    return NULL;
  }

  for (size_t i = 0; i < phnum; i++) {
    parse_pg_header(buf + i * phentsize, &phdrs[i], parser->endianess);
  }

  free(buf);
  Phdr_Table *table = malloc(sizeof(Phdr_Table));
  if (table == NULL) {
    fprintf(stderr, "Malloc error\n");
    free(phdrs);
    return NULL;
  }

  table->entries = phdrs;
  table->length = phnum;

  return table;
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

void print_program_header(Phdr_Table *table) {
  Elf64_Phdr *phdrs = table->entries;

  for (size_t i = 0; i < table->length; i++) {
    char flags[4];
    pghdr_flags(phdrs[i].p_flags, flags);

    printf("p_type: %s\n", pt_type(phdrs[i].p_type));
    printf("p_flags: %s\n", flags);
    printf("p_offset: 0x%" PRIx64 "\n", phdrs[i].p_offset);
    printf("p_vaddr: 0x%" PRIx64 "\n", phdrs[i].p_vaddr);
    printf("p_paddr: 0x%" PRIx64 "\n", phdrs[i].p_paddr);
    printf("p_filesz: 0x%" PRIx64 "\n", phdrs[i].p_filesz);
    printf("p_memsz: 0x%" PRIx64 "\n", phdrs[i].p_memsz);
    printf("p_align: 0x%" PRIx64 "\n", phdrs[i].p_align);
  }
}

void print_program_header_fmt(Phdr_Table *table) {
  printf("%-16s %-10s %-10s %-10s %-10s %-10s %-4s %-8s \n", "Type", "Offset",
         "VirtAddr", "PhysAddr", "Filesz", "Memsz", "Flag", "Align");

  Elf64_Phdr *phdrs = table->entries;
  for (size_t i = 0; i < table->length; i++) {
    Elf64_Phdr phdr = phdrs[i];
    const char *type = pt_type(phdr.p_type);
    char flags[4];
    pghdr_flags(phdr.p_flags, flags);

    printf("%-16s 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64
           " 0x%08" PRIx64 " %-3s 0x%" PRIx64 "\n",
           type, phdr.p_offset, phdr.p_vaddr, phdr.p_paddr, phdr.p_filesz,
           phdr.p_memsz, flags, phdr.p_align);
  }
}

void free_phdr_table(Phdr_Table *table) {
  if (table == NULL)
    return;

  free(table->entries);
  free(table);
}
