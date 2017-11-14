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

#define CONFIG "cchip8.ini"
#define MAXBUF 1024

char *_rom_path;

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

void on_key(S2D_Event e) {

    if (e.type == S2D_KEY_DOWN && strcmp(e.key, "Escape") == 0) {
        exit(1);
    }
    char c = tolower(e.key[0]);

    switch (e.type) {
        case S2D_KEY_HELD:
        case S2D_KEY_DOWN:
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

        case S2D_KEY_UP:
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

void handle_args(int argc, char *args[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(args[i], "--rom") == 0) {
            _rom_path = malloc(strlen(args[i + 1]) + 1);
            memcpy(_rom_path, args[i + 1], strlen(args[i + 1]) + 1);
            printf("Loading ROM [%s]\n", _rom_path);
            i++;
        }
    }
}

int main(int argc, char *args[]) {
    handle_args(argc, args);

    S2D_Window *window = S2D_CreateWindow(
            "CChip8", 640, 480, cpu_cycle, render, 0
    );

    window->on_key = on_key;

    int ret = rom_load(_rom_path);

    if (ret != 0) {
        printf("Error Opening ROM File\n");
        return 1;
    }

    printf("Initialising\n");
    cpu_init();
    power_up(rom_getbytes());

    S2D_Show(window);
    S2D_FreeWindow(window);

    return 0;
}