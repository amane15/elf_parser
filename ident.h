#ifndef ELF_IDENT_H
#define ELF_IDENT_H

#include <stdbool.h>
#include <stdio.h>

#define ELF_IDENT_SIZE 16

typedef struct {
    unsigned char ei_class;
    unsigned char ei_data;
    unsigned char elf_version;
    unsigned char ei_osabi;
    unsigned char ei_abiversion;
} ELFIdent;

typedef enum {
    ELF32 = 1,
    ELF64,
} ELFClass;

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

void print_ident_info(ELFIdent* info);
void print_osabi(unsigned char ei_osabi);
bool parse_ident_data(FILE* fp, ELFIdent* ident);

#endif
