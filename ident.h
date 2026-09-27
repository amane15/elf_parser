#ifndef ELF_IDENT_H
#define ELF_IDENT_H

#include <endian.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ELF_IDENT_SIZE 16

typedef struct {
  unsigned char ei_class;
  unsigned char ei_data;
  unsigned char ei_version;
  unsigned char ei_osabi;
  unsigned char ei_abiversion;
} ELFIdent;

typedef struct {
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} ELFHeaderRest;

typedef enum {
  ELFUNKNOWN,
  ELF32,
  ELF64,
} ELFClass;

typedef enum {
  LittleEndian = 1,
  BigEndian,
} ELFDataEnc;

typedef enum {
  ELFOSABI_SYSV = 0,
  ELFOSABI_HPUX = 1,
  ELFOSABI_NETBSD = 2,
  ELFOSABI_LINUX = 3,
  ELFOSABI_HURD = 4,
  ELFOSABI_SOLARIS = 6,
  ELFOSABI_AIX = 7,
  ELFOSABI_IRIX = 8,
  ELFOSABI_FREEBSD = 9,
  ELFOSABI_TRU64 = 10,
  ELFOSABI_MODESTO = 11,
  ELFOSABI_OPENBSD = 12,
  ELFOSABI_OPENVMS = 13,
  ELFOSABI_NSK = 14,
  ELFOSABI_AROS = 15,
  ELFOSABI_FENIXOS = 16,
  ELFOSABI_CLOUDABI = 17,
  ELFOSABI_OPENVOS = 18,
} ELFOsAbi;

typedef enum {
  ELFETENONE,
  ELFETREL,
  ELFETEXEC,
  ELFETDYN,
  ELFETCORE,
  ELFETLOPROC = 0xff00,
  ELFETHIPROC = 0xffff,
} ELFEtype;

void print_ident_info(ELFIdent *info);
void print_osabi(unsigned char ei_osabi);
bool parse_ident_data(FILE *fp, ELFIdent *ident);
bool parse_rest_header(FILE *fp, ELFIdent *ident, ELFHeaderRest *rest);
void print_elf_rest_header(ELFHeaderRest *header);

#endif
