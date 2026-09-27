#include "ident.h"
#include "elf_types.h"
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

static void print_e_type(uint16_t type) {
  printf("Type: ");
  switch (type) {
  case ET_REL:
    printf("Relocatable file\n");
    break;
  case ET_EXEC:
    printf("Executable file\n");
    break;
  case ET_DYN:
    printf("Shared object file\n");
    break;
  case ET_CORE:
    printf("Core file\n");
    break;
  default:
    printf("Unknown e_type: %u\n", type);
  }
}

static void print_e_machine(uint16_t machine) {
  printf("Machine: ");
  switch (machine) {
  case EM_386:
    printf("Intel 80386 (32-bit x86)\n");
    break;
  case EM_ARM:
    printf("ARM 32-bit\n");
    break;
  case EM_X86_64:
    printf("AMD x86-64 (64-bit AMD/Intel Architecture)\n");
    break;
  case EM_AARCH64:
    printf("ARM 64-bit\n");
    break;
  case EM_PPC:
    printf("Power PC 32-bit\n");
    break;
  case EM_PPC64:
    printf("Power PC 64-bit\n");
    break;
  case EM_MIPS:
    printf("MIPS R3000 / general MIPS\n");
    break;
  case EM_RISCV:
    printf("RISC-V\n");
    break;

  default:
    printf("Unknown machine type: %u\n", machine);
  }
}

void print_elf_rest_header(ELFHeaderRest *rest) {
  print_e_type(rest->e_type);
  print_e_machine(rest->e_machine);
  printf("Version: 0x%" PRIx32 "\n", rest->e_version);
  printf("Entry point address: 0x%" PRIx64 "\n", rest->e_entry);
  printf("Program header table offset: %" PRIu64 " (bytes into file)\n",
         rest->e_phoff);
  printf("Section header table offset: %" PRIu64 " (bytes into file)\n",
         rest->e_shoff);
  printf("EFlags: 0x%" PRIx32 "\n", rest->e_flags);
  printf("Elf header size: %u (in bytes)\n", rest->e_ehsize);
  printf("Size of program header: %u (bytes)\n", rest->e_phentsize);
  printf("Number of program headers: %u\n", rest->e_phnum);
  printf("Size of section header: %u (bytes)\n", rest->e_shentsize);
  printf("Number of section headers: %u\n", rest->e_shnum);
  printf("Section header string table index: %u\n", rest->e_shstrndx);
}
