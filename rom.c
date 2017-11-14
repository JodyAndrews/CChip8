#include <stdio.h>
#include "rom.h"

/// Maximum RAM availability for CHIP8
const short max_ram = 4096;
/// Start location for most ROMS
const short start_loc = 0x200;
/// File bytes in memory
unsigned char _memory[max_ram];

/// Load the binary contents of a Chip8 ROM into memory
/// \param filename
/// \return 1 for failure, 0 for success.
int rom_load(const char *filename) {
    FILE *file;
    // Open file as read for bytes
    file = fopen(filename, "rb");
    if (NULL == file) {
        fprintf(stderr, "Unable to open file [%s]\n", filename);
        return 1;
    } else {
        fprintf(stdout, "Opened ROM file [%s]\n", filename);
    }
    // Read at start location to end location
    size_t s = fread(_memory + start_loc, 1, max_ram - start_loc, file);
    // Courtesy check size of read bytes
    if (0 == s) {
        fprintf(stderr, "Read zero bytes opening file [%s]\n", filename);
        return 1;
    }
    // Close file
    fprintf(stdout, "Closing ROM file [%s]\n", filename);
    fclose(file);
    // Return success
    return 0;
}

/// Return memory
unsigned char *rom_getbytes(void) {
    return _memory;
}