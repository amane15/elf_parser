#include "elf_dynamic.h"
#include "elf_program.h"
#include "elf_reader.h"
#include "parser.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Dyn_Table *parse_dyn_tb(Parser *parser, Phdr_Table *phdrs_table) {
  ELF64_Dyn *dyn_entries = NULL;
  unsigned char *buf = NULL;
  Elf64_Phdr phdr;
  size_t num_dyn_entries = 0;
  uint64_t section_offset = 0;

  for (size_t i = 0; i < phdrs_table->length; i++) {
    phdr = phdrs_table->entries[i];
    if (phdr.p_type != PT_DYNAMIC) {
      continue;
    }

    section_offset = phdr.p_offset;
    if (fseek(parser->file, (long)phdr.p_offset, SEEK_SET) != 0) {
      fprintf(stderr, "Failed to seek PT_DYNAMIC\n");
      return NULL;
    }

    size_t num_of_entries = phdr.p_filesz / sizeof(ELF64_Dyn);

    dyn_entries = malloc(sizeof(ELF64_Dyn) * num_of_entries);
    if (dyn_entries == NULL) {
      fprintf(stderr, "Malloc error\n");
      return NULL;
    }

    buf = malloc(phdr.p_filesz);
    if (buf == NULL) {
      fprintf(stderr, "Malloc error\n");
      free(dyn_entries);
      return NULL;
    }

    if (fread(buf, 1, phdr.p_filesz, parser->file) != phdr.p_filesz) {
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
    num_dyn_entries = length;
    break;
  }

  Dyn_Table *table = (Dyn_Table *)malloc(sizeof(Dyn_Table));
  if (table == NULL) {
    fprintf(stderr, "Malloc error\n");
    free(buf);
    free(dyn_entries);
    return NULL;
  }

  table->entries = dyn_entries;
  table->length = num_dyn_entries;
  table->section_offset = section_offset;

  free(buf);

  return table;
}

void print_dyn_tb(Dyn_Table *table) {
  printf("Dynamic section at the offset %" PRIx64 " contains %zu"
         " entries\n",
         table->section_offset, table->length);
  for (size_t i = 0; i < table->length; i++) {
    printf("Tag: %" PRIx64 "\n", table->entries[i].d_tag);
    printf("Val: %" PRIx64 "\n", table->entries[i].d_val);
  }
}

void free_dyn_table(Dyn_Table *table) {
  if (table == NULL)
    return;

  free(table->entries);
  free(table);
}

static char *dt_tag_type(uint64_t d_val) {
  switch (d_val) {
  case DT_NULL:
    return "(NULL)";
  case DT_NEEDED:
    return "(NEEDED)";
  case DT_PLTRELSZ:
    return "(PLTRELSZ)";
  case DT_PLTGOT:
    return "(PLTGOT)";
  case DT_HASH:
    return "(HASH)";
  case DT_STRTAB:
    return "(STRTAB)";
  case DT_SYMTAB:
    return "(SYMTAB)";
  case DT_RELA:
    return "(RELA)";
  case DT_RELASZ:
    return "(RELASZ)";
  case DT_RELAENT:
    return "(RELAENT)";
  case DT_STRSZ:
    return "(STRSZ)";
  case DT_SYMENT:
    return "(SYMENT)";
  case DT_INIT:
    return "(INIT)";
  case DT_FINI:
    return "(FINI)";
  case DT_SONAME:
    return "(SONAME)";
  case DT_RPATH:
    return "(RPATH)";
  case DT_SYMBOLIC:
    return "(SYMBOLIC)";
  case DT_REL:
    return "(REL)";
  case DT_RELSZ:
    return "(RELSZ)";
  case DT_RELENT:
    return "(RELENT)";
  case DT_PLTREL:
    return "(PLTREL)";
  case DT_DEBUG:
    return "(DEBUG)";
  case DT_TEXTREL:
    return "(TEXTREL)";
  case DT_JMPREL:
    return "(JMPREL)";
  case DT_BIND_NOW:
    return "(BIND_NOW)";
  case DT_INIT_ARRAY:
    return "(INIT_ARRAY)";
  case DT_FINIARRAY:
    return "(FINI_ARRAY)";
  case DT_INITARRYSZ:
    return "(INIT_ARRAYSZ)";
  case DT_FINIARRAYSZ:
    return "(FINI_ARRAYSZ)";
  case DT_GNU_HASH:
    return "(GNU_HASH)";
  case DT_FLAGS_1:
    return "(FLAGS_1)";
  case DT_VERNEED:
    return "(VERNEED)";
  case DT_VERNEEDNUM:
    return "(VENEEDNUM)";
  case DT_VERSYM:
    return "(VERSYM)";
  case DT_RELACOUT:
    return "(RELACOUNT)";
  default:
    if (d_val >= DT_LOPROC && d_val <= DT_HIPROC) {
      return "(PROCESSOR SPECIFIC)";
    }
    return "";
  }
}

void print_dyn_tb_fmt(Dyn_Table *table) {
  printf("Dynamic section at the offset %" PRIx64 " contains %zu"
         " entries\n",
         table->section_offset, table->length);

  printf("%-16s %-16s %-10s\n", "Tag", "Type", "Name/Value");
  for (size_t i = 0; i < table->length; i++) {
    ELF64_Dyn entry = table->entries[i];
    printf("0x%016" PRIx64 " %-16s 0x%" PRIx64 "\n", entry.d_tag,
           dt_tag_type(entry.d_tag), entry.d_val);
  }
}
