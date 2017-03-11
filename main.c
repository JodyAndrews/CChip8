#include <stdio.h>
#include <simple2d.h>
#include "rom.h"
#include "cpu.h"
int displaySize = 32*64;

void rect(int x, int y, int size, int r, int g, int b) 
{
	S2D_DrawQuad(	x, y, r, g, b, 1,
					x + size, y, r, g, b, 1,
					x + size, y + size, r, g, b, 1,
					x, y + size, r, g, b, 1);
}
void render() {
	int pixel_size = 10;
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

//cc *.c `simple2d --libs` -o main
int main(int argc, char *argv[]) {
	printf("Starting...\n");
	if (argc <= 1) 
	{
		printf("ROM File must be specified");
		return 0;
	}
	S2D_Window *window = S2D_CreateWindow(
		"CChip8", 640, 480, update, render, 0
	);

	rom_load(argv[1]);
	cpu_init();
	power_up(rom_getbytes());
	
	
	S2D_Show(window);
	S2D_FreeWindow(window);

  return 0;
}

