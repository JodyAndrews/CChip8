/*
 * main.c
 * Simple2D based Chip8 Interpreter
 * 
 * cc *.c `simple2d --libs` -o cchip8
 */

#include <stdio.h>
#include <simple2d.h>
#include "rom.h"
#include "cpu.h"
#include "config.h"

#define CONFIG "cchip8.ini"
#define MAXBUF 1024
#define DELIM "="

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

			unsigned char value;
			value = strtol(cfline, NULL, 0);
			
			if (i == 0) {
				configstruct.pixel_size = value;
				printf("\npixel size [%d]", configstruct.pixel_size);
			} else if (i == 1) {
				configstruct.verbose = value;
				printf(" | verbose [%d]", configstruct.verbose);
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

void on_key(S2D_Event e, const char *key) {

	if (e == S2D_KEYDOWN && strcmp(key,"Escape") == 0) {
		exit(1);
	}
	char c = tolower(key[0]);

	switch (e) {
		case S2D_KEY:
		case S2D_KEYDOWN:
			if (c == '1') {
				set_key(1, 1);
			} else if (c == '2') {
				set_key(2, 1);
			} else if (c == '3') {
				set_key(3, 1);
			} else if (c == '4') {
				set_key(12, 1);
			} else if (c == 'q') {
				set_key(4, 1);
			} else if (c == 'w') {
				set_key(5, 1);
			} else if (c == 'e') {
				set_key(6, 1);
			} else if (c == 'r') {
				set_key(13, 1);
			} else if (c == 'a') {
				set_key(7, 1);
			} else if (c == 's') {
				set_key(8, 1);
			} else if (c == 'd') {
				set_key(9, 1);
			} else if (c == 'f') {
				set_key(14, 1);
			} else if (c == 'z') {
				set_key(10, 1);
			} else if (c == 'x') {
				set_key(0, 1);
			} else if (c == 'c') {
				set_key(11, 1);
			} else if (c == 'v') {
				set_key(15, 1);
			}
		  break;

		case S2D_KEYUP:
			if (c == '1') {
				set_key(1, 0);
			} else if (c == '2') {
				set_key(2, 0);
			} else if (c == '3') {
				set_key(3, 0);
			} else if (c == '4') {
				set_key(12, 0);
			} else if (c == 'q') {
				set_key(4, 0);
			} else if (c == 'w') {
				set_key(5, 0);
			} else if (c == 'e') {
				set_key(6, 0);
			} else if (c == 'r') {
				set_key(13, 0);
			} else if (c == 'a') {
				set_key(7, 0);
			} else if (c == 's') {
				set_key(8, 0);
			} else if (c == 'd') {
				set_key(9, 0);
			} else if (c == 'f') {
				set_key(14, 0);
			} else if (c == 'z') {
				set_key(10, 0);
			} else if (c == 'x') {
				set_key(0, 0);
			} else if (c == 'c') {
				set_key(11, 0);
			} else if (c == 'v') {
				set_key(15, 0);
			}
		  break;
	}
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
		"CChip8", configstruct.pixel_size * 64, configstruct.pixel_size * 32, cpu_cycle, render, 0
	);
	
	window->on_key = on_key;

	int ret = rom_load(argv[1]);
	if (ret != 0)
		return 0;
		
	cpu_init(&configstruct);
	power_up(rom_getbytes());
	
	S2D_Show(window);
	S2D_FreeWindow(window);

  return 0;
}