#include <stdio.h>
#include "rom.h"

unsigned char _memory[4096];

int rom_load(const char *filename) {
    FILE *file;
    file = fopen(filename, "rb");
    if (NULL == file) {
        fprintf(stderr, "Unable to open file [%s]\n", filename);
        return 1;
    } else {
        fprintf(stdout, "Opened ROM file [%s]\n", filename);
    }
    size_t s = fread(_memory + 0x200, 1, 4096 - 0x200, file);
    if (0 == s) {
        fprintf(stderr, "Read zero bytes opening file [%s]\n", filename);
        return 1;
    }

    fprintf(stdout, "Closing ROM file [%s]\n", filename);
    fclose(file);
    return 0;
}

unsigned char *rom_getbytes(void) {
    return _memory;
}