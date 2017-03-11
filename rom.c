#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rom.h"

char *ret;
char *buffer;
unsigned char _memory[4096];

void ReadFile(const char *name)
{
	FILE *file;
	
	unsigned long fileLen;

	//Open file
	file = fopen(name, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", name);
		return;
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer=(char *)malloc(fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(file);
		return;
	}

	//Read file contents into buffer
	//fread(buffer, fileLen, 1, file);
	//fclose(file);
	fread(_memory + 0x200, 1, 4096 - 0x200, file);
	//Do what ever with buffer

//	free(buffer);
}

int rom_load(const char *filename)
{
	ReadFile(filename);
	/*
	printf("Loading %s\n", filename);
	
	FILE *fl = fopen(filename, "r");  
    fseek(fl, 0, SEEK_END);  
    long len = ftell(fl);  
    char *ret = malloc(len);  
    fseek(fl, 0, SEEK_SET);  
    fread(ret, 1, len, fl);  
    fclose(fl);*/
	
/*
	FILE *fileptr;

	long filelen;

	fileptr = fopen("INVADERS", "rb");  // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	bytes = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(bytes, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file
*/
	return 0;
}

unsigned char *rom_getbytes(void)
{
	//unsigned int i = 0;
	//printf("%s",ret);
	//for (i = 0; i < 80; i++) 
	//	printf("%c\n", ret[i]);
	return _memory;
}