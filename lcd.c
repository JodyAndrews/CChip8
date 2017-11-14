#include "lcd.h"
#include "cpu.h"
#include <simple2d.h>

void rect(int x, int y, int size, int r, int g, int b) {
    S2D_DrawQuad(x, y, r, g, b, 1,
                 x + size, y, r, g, b, 1,
                 x + size, y + size, r, g, b, 1,
                 x, y + size, r, g, b, 1);
}

void render() {
    int pixel_size = 10;

    unsigned char *display = get_display();

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            int g = 0;
            if (display[(y * 64) + x] != 0x0000) {
                g = 1;
            }
            rect(x * pixel_size, y * pixel_size, pixel_size, 0, g, 0);
        }
    }
}