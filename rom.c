#include <stdio.h>
#include <string.h>
#include "rom.h"
#include "config.h"

unsigned char _memory[4096];

int rom_load(const char *filename, struct config *config)
{
	FILE *file;
	char *pch;
	pch = strrchr(filename, '/');
	if (!pch) {
		char newFilename[strlen(filename) + strlen(config->rom_path) + 1];
		strcpy(newFilename, config->rom_path);
		strcat(newFilename, "/");
		strcat(newFilename, filename);

		printf("\nNo folder specified. Using [%s]\n", config->rom_path);
		file = fopen(newFilename, "rb");
	} else {
		file = fopen(filename, "rb");
	}
	
	if (!file)
	{
		fprintf(stderr, "\nUnable to open file %s", filename);
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