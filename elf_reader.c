#include <stdio.h>

#define ELF_IDENT_START 16

typedef struct {
    unsigned char ei_class;
    unsigned char ei_data;
    unsigned char elf_version;
    unsigned char os_abi;
    unsigned char abi_version;
} ELFStartInfo;

void print_begin_info(ELFStartInfo info);

int main(int argc, char *argv[]) {
    unsigned char start_buffer[ELF_IDENT_START];

    if (argc < 2) {
        fprintf(stderr, "Error: Missing file argument\n");
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    size_t start_bytes_read = fread(start_buffer, 1, ELF_IDENT_START, file);
    if (start_bytes_read != ELF_IDENT_START) {
        fprintf(stderr, "Error: file is too small to contain an elf header\n");
        fclose(file);
        return 1;
    }

    if (start_buffer[0] != 0x7f || start_buffer[1] != 'E' ||
        start_buffer[2] != 'L' || start_buffer[3] != 'F') {
        fprintf(stderr, "Given file is not elf file\n");
        fclose(file);
        return 1;
    }

    ELFStartInfo elf_ident_header;
    elf_ident_header.ei_class = start_buffer[4];
    elf_ident_header.ei_data = start_buffer[5];
    elf_ident_header.elf_version = start_buffer[6];
    elf_ident_header.os_abi = start_buffer[7];
    elf_ident_header.abi_version = start_buffer[8];

    print_begin_info(elf_ident_header);

    fclose(file);
    return 0;
}

void print_begin_info(ELFStartInfo info) {
    printf("ELF Class: ");
    switch (info.ei_class) {
    case 1:
        printf("ELF32\n");
        break;
    case 2:
        printf("ELF64\n");
        break;

    default:
        printf("Unknown: (%u)\n", info.ei_class);
        break;
    }

    printf("Endianess: ");
    switch (info.ei_data) {
    case 1:
        printf("Little Endian\n");
        break;
    case 2:
        printf("Big Endian\n");
        break;
    default:
        printf("Unknown (%u)\n", info.ei_data);
    }

    printf("ELF Version: %u\n", info.elf_version);
    if (info.os_abi == 0) {
        printf("ABI: System V ABI\n");
    }

    printf("ABI Version: %u\n", info.abi_version);
}
