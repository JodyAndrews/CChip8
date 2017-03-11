/*
 * main.c
 * Simple2D based Chip8 Interpreter
 * 
 * cc *.c `simple2d --libs` -o main
 */

#include <stdio.h>
#include <simple2d.h>
#include "rom.h"
#include "cpu.h"

#define CONFIG "cchip8.ini"
#define MAXBUF 1024
#define DELIM "="

int displaySize = 32*64;

struct config
{
	unsigned char pixel_size;
	unsigned char verbose;
	int window_width;
	int window_height;
};

struct config configstruct;

struct config get_config(char *filename)
{
	struct config configstruct;
	FILE *file = fopen(filename, "r");
	
	if (file != NULL)
	{
		char line[MAXBUF];
		int i = 0;
		
		while(fgets(line, sizeof(line), file) != NULL)
		{
			char *cfline;
			cfline = strstr((char *)line,DELIM);
			cfline = cfline + strlen(DELIM);
			char str_buffer[MAXBUF];
			memcpy(str_buffer, cfline, strlen(cfline));
			
			if (i == 0) {
				configstruct.pixel_size = atoi(str_buffer);
				printf("\npixel size [%d]", configstruct.pixel_size);
			} else if (i == 1) {
				configstruct.verbose = atoi(str_buffer);
				printf(" | verbose [%d]", configstruct.verbose);
			} else if (i == 2) {
				configstruct.window_width = atoi(str_buffer);
				printf(" | window width [%d]", configstruct.window_width);
			} else if (i == 3) {
				configstruct.window_height = atoi(str_buffer);
				printf(" | window height [%d]", configstruct.window_height);
			}
			
			i++;
		}
		fclose(file);
		printf("\n");
	} else {
		fclose(file);
	}
	
	return configstruct;
}

void rect(int x, int y, int size, int r, int g, int b) 
{
	S2D_DrawQuad(	x, y, r, g, b, 1,
					x + size, y, r, g, b, 1,
					x + size, y + size, r, g, b, 1,
					x, y + size, r, g, b, 1);
}

void render() {
	int pixel_size = configstruct.pixel_size;
	
	unsigned char *display = get_display();

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			int g = 0;
			if (display[(y * 64) + x] != 0x0000)
			{
				g = 1;
			}
			rect(x * pixel_size, y * pixel_size, pixel_size, 0, g, 0);
		}
	}
}

void update() {
	cpu_cycle();
}

int main(int argc, char *argv[]) {
	printf("starting...\n");
	printf("reading config from %s", CONFIG);
	
	configstruct = get_config(CONFIG);
	
	if (argc <= 1) 
	{
		printf("rom file must be specified");
		return 0;
	}
	
	if (argc == 3 && strcmp(argv[2], "-v") == 0)
	{
		if (configstruct.verbose != 1) {
			configstruct.verbose = 1;
			printf("verbose level overridden, now [%d]\n", configstruct.verbose);
		}
	}
	
	S2D_Window *window = S2D_CreateWindow(
		"CChip8", configstruct.window_width, configstruct.window_height, update, render, 0
	);

	rom_load(argv[1]);
	cpu_init();
	power_up(rom_getbytes());
	
	S2D_Show(window);
	S2D_FreeWindow(window);

  return 0;
}