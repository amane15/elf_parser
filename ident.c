#include "ident.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool parse_ident_data(FILE *file, ELFIdent *ident) {
  unsigned char start_buffer[ELF_IDENT_SIZE];

  size_t start_bytes_read = fread(start_buffer, 1, ELF_IDENT_SIZE, file);
  if (start_bytes_read != ELF_IDENT_SIZE) {
    fprintf(stderr, "Error: file is too small to contain an elf header\n");
    return false;
  }

  if (start_buffer[0] != 0x7f || start_buffer[1] != 'E' ||
      start_buffer[2] != 'L' || start_buffer[3] != 'F') {
    fprintf(stderr, "Given file is not elf file\n");
    return false;
  }

  ident->ei_class = start_buffer[4];
  ident->ei_data = start_buffer[5];
  ident->ei_version = start_buffer[6];
  ident->ei_osabi = start_buffer[7];
  ident->ei_abiversion = start_buffer[8];

  return true;
}

void print_ident_info(ELFIdent *info) {
  printf("ELF Class: ");
  switch (info->ei_class) {
  case ELF32:
    printf("ELF32\n");
    break;
  case ELF64:
    printf("ELF64\n");
    break;

  default:
    printf("Unknown: (%u)\n", info->ei_class);
    break;
  }

  printf("Endianess: ");
  switch (info->ei_data) {
  case LittleEndian:
    printf("Little Endian\n");
    break;
  case BigEndian:
    printf("Big Endian\n");
    break;
  default:
    printf("Unknown (%u)\n", info->ei_data);
  }

  printf("ELF Version: %u\n", info->ei_version);
  print_osabi(info->ei_osabi);

  printf("ABI Version: %u\n", info->ei_abiversion);
}

void print_osabi(unsigned char ei_osabi) {
  // ident.c
  printf("ABI: ");
  switch (ei_osabi) {
  case ELFOSABI_SYSV:
    printf("System V\n");
    break;
  case ELFOSABI_HPUX:
    printf("HP-UX\n");
    break;
  case ELFOSABI_NETBSD:
    printf("NetBSD\n");
    break;
  case ELFOSABI_LINUX:
    printf("Linux\n");
    break;
  case ELFOSABI_HURD:
    printf("GNU Hurd\n");
    break;
  case ELFOSABI_SOLARIS:
    printf("Solaris\n");
    break;
  case ELFOSABI_AIX:
    printf("AIX\n");
    break;
  case ELFOSABI_IRIX:
    printf("IRIX\n");
    break;
  case ELFOSABI_FREEBSD:
    printf("FreeBSD\n");
    break;
  case ELFOSABI_TRU64:
    printf("Tru64\n");
    break;
  case ELFOSABI_MODESTO:
    printf("Novell Modesto\n");
    break;
  case ELFOSABI_OPENBSD:
    printf("OpenBSD\n");
    break;
  case ELFOSABI_OPENVMS:
    printf("OpenVMS\n");
    break;
  case ELFOSABI_NSK:
    printf("NonStop Kernel\n");
    break;
  case ELFOSABI_AROS:
    printf("AROS\n");
    break;
  case ELFOSABI_FENIXOS:
    printf("FenixOS\n");
    break;
  case ELFOSABI_CLOUDABI:
    printf("CloudABI\n");
    break;
  case ELFOSABI_OPENVOS:
    printf("Open VOS\n");
    break;
  default:
    if (ei_osabi >= 64)
      printf("Architecture-specific (%u)\n", ei_osabi);
    else
      printf("Unknown (%u)\n", ei_osabi);
    break;
  }
}

static uint16_t read_u16(unsigned char *buf, ELFDataEnc endianess) {
  return endianess == LittleEndian ? (uint16_t)(buf[0] | buf[1] << 8)
                                   : (uint16_t)(buf[0] << 8 | buf[1]);
}

static uint32_t read_u32(unsigned char *buf, ELFDataEnc endianess) {
  return endianess == LittleEndian
             ? (uint32_t)(buf[0] | buf[1] << 8 | buf[2] << 16) |
                   (uint32_t)(buf[3] << 24)
             : (uint32_t)((uint32_t)(buf[0] << 24) | buf[1] << 16 |
                          buf[2] << 8 | buf[3]);
}

static uint64_t read_u64(unsigned char *buf, ELFDataEnc endianess) {
  uint64_t data = 0;
  if (endianess == LittleEndian) {
    for (int i = 7; i >= 0; i--) {
      data = (data << 8) | buf[i];
    }
  } else {
    for (int i = 0; i < 8; i++) {
      data = (data << 8) | buf[i];
    }
  }

  return data;
}

bool parse_rest_header(FILE *fp, ELFIdent *ident, ELFHeaderRest *rest) {
  ELFDataEnc byte_order =
      ident->ei_data == LittleEndian ? LittleEndian : BigEndian;
  size_t rem_header_size = (ident->ei_class == ELF64) ? 48 : 36;

  unsigned char buffer[48];
  if (fread(buffer, 1, rem_header_size, fp) != rem_header_size) {
    fprintf(stderr, "File is too small to contain an elf header\n");
    return false;
  }

  size_t offset = 0;
  rest->e_type = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_machine = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_version = read_u32(buffer + offset, byte_order);
  offset += 4;

  if (ident->ei_class == ELF64) {
    rest->e_entry = read_u64(buffer + offset, byte_order);
    offset += 8;

    rest->e_phoff = read_u64(buffer + offset, byte_order);
    offset += 8;

    rest->e_shoff = read_u64(buffer + offset, byte_order);
    offset += 8;
  } else {
    rest->e_entry = read_u32(buffer + offset, byte_order);
    offset += 4;

    rest->e_phoff = read_u32(buffer + offset, byte_order);
    offset += 4;

    rest->e_shoff = read_u32(buffer + offset, byte_order);
    offset += 4;
  }

  rest->e_flags = read_u32(buffer + offset, byte_order);
  offset += 4;

  rest->e_ehsize = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_phentsize = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_phnum = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_shentsize = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_shnum = read_u16(buffer + offset, byte_order);
  offset += 2;

  rest->e_shstrndx = read_u16(buffer + offset, byte_order);
  offset += 2;

  return true;
}

void print_elf_rest_header(ELFHeaderRest *rest) {
  printf("Type: %u\n", rest->e_type);
  printf("Machine: %u\n", rest->e_machine);
  printf("Version: 0x%" PRIx32 "\n", rest->e_version);
  printf("Entry: 0x%" PRIx64 "\n", rest->e_entry);
  printf("Phoff: %lu\n", rest->e_phoff);
  printf("Shoff: %lu\n", rest->e_shoff);
  printf("EFlags: 0x%" PRIx32 "\n", rest->e_flags);
  printf("Ehsize: %u\n", rest->e_ehsize);
  printf("Phentize: %u\n", rest->e_phentsize);
  printf("Phnum: %u\n", rest->e_phnum);
  printf("Shentsize: %u\n", rest->e_shentsize);
  printf("Shnum: %u\n", rest->e_shnum);
  printf("Shstridx: %u\n", rest->e_shstrndx);
}
