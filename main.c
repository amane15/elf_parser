#include "ident.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

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

    ELFIdent elf_ident_header;

    bool is_parsed = parse_ident_data(file, &elf_ident_header);
    if (!is_parsed) {
        fprintf(stderr, "Error: while parsing idenity header\n");
        fclose(file);
        return 1;
    }

    print_ident_info(&elf_ident_header);

    fclose(file);
    return 0;
}
