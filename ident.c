#include "ident.h"
#include <stdbool.h>
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
    ident->elf_version = start_buffer[6];
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
    case 1:
        printf("Little Endian\n");
        break;
    case 2:
        printf("Big Endian\n");
        break;
    default:
        printf("Unknown (%u)\n", info->ei_data);
    }

    printf("ELF Version: %u\n", info->elf_version);
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
