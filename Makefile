all:
	gcc -o ./bin/relf main.c ident.c -Wall -Wextra

debug:
	gcc -g -O0 -Wall -Wextra main.c ident.c -o ./bin/relf_debug
