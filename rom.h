#ifndef ROM_H
#define ROM_H

#include "config.h"

int rom_load(const char *, struct config *config);
unsigned char *rom_getbytes(void);

#endif