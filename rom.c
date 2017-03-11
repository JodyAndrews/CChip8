#include <stdio.h>
#include "rom.h"

unsigned char _memory[4096];

int rom_load(const char *filename)
{
	FILE *file;

	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", filename);
		return 1;
	}

	fread(_memory + 0x200, 1, 4096 - 0x200, file);

	fclose(file);

	return 0;
}

unsigned char *rom_getbytes(void)
{
	return _memory;
}