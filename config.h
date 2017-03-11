#ifndef CONFIG_H
#define CONFIG_H
#define MAXBUF 1024
struct config
{
	unsigned char pixel_size;
	unsigned char verbose;
	char rom_path[MAXBUF];
};

#endif