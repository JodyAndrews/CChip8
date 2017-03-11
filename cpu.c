#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "config.h"
#include <simple2d.h>

unsigned char _memory[4096];
unsigned char _v[16];
int _pc;
unsigned char _display[32 * 64];
bool _poweredUp = false;
bool _halted = false;
unsigned short _i;
unsigned short _stack[16];
short _sp;
unsigned char _st;
unsigned char _dt;
int cycle_count = 0;
struct config *_config;
int _keys[16];
S2D_Sound *snd1;

void cpu_init(struct config *config) 
{
	_config = config;
	srand(time(NULL));
	snd1 = S2D_CreateSound("media/beep.wav");
	_pc = 0x200;
	_sp &= 0;
	_dt = 0;
	_st = 0;
	for (int i = 0; i < 0x10; i++)
	{
		_v[i] = 0;
		_stack[i] = 0;
		_keys[i] = 0;
	}
}

void power_up(unsigned char *bytes) {

	unsigned char fonts[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	unsigned int i;
	for (i = 0; i < 0x50; i++) {
		bytes[i] = fonts[i];
	}
	
	for (i = 0; i < (4096); ++i) {
		_memory[i] = bytes[i];
	}
}

void set_key(int index, int val)
{
	_keys[index] = val;
}

/// <summary>
/// Clears the display buffer
/// </summary>
void cls()
{
	for (int i = 0; i < (sizeof(_display) / sizeof(unsigned char)); i++)
	{
		_display[i] = 0x0000;
	}
}

/// <summary>
/// Return from a subroutine
/// </summary>
void ret()
{
	_pc = _stack[--_sp];
}

/// <summary>
/// JP kkk
/// 
/// Jumps to address kkk
/// </summary>
/// <param name="kkk">kkk</param>
void jp(unsigned short kkk)
{
	_pc = kkk;
}

/// <summary>
/// Calls subroutine at the specified address and stores previous address on stack, advancing stack pointer.
/// </summary>
/// <param name="addr">addr</param>
void call(unsigned short addr)
{
	_stack[_sp] = _pc;
	_sp++;
	_pc = addr;
}

/// <summary>
/// SE Vx, kk
/// 
/// Skip next instruction if Vx is equal to kk
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void se(unsigned char ax, unsigned char kk)
{
	if (_v[ax] == kk) 
	{
		_pc += 2;
	}
}

/// <summary>
/// SNE Vx, kk
/// 
/// Skips next instruction if Vx does not equal kk
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void sne(unsigned char ax, unsigned char kk)
{
	if (_v[ax] != kk) {
		_pc += 2;
	}
}

/// <summary>
/// Loads value kk into address ax
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void ld(unsigned char ax, unsigned char kk)
{
	_v[ax] = kk;
}

/// <summary>
/// ADD Vx, kk
/// 
/// Adds Vx to kk and stores result at Vx
/// 
/// Sets carry register if necessary
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void add(unsigned char ax, unsigned char kk)
{
	_v[0xf] = _v[ax] + kk > 255 ? 1 : 0;
	_v[ax] += kk;
}

/// <summary>
/// SUB Vx, kk
/// 
/// Subtracts kk from Vx and stores value at Vx
/// 
/// Sets Vf if borrow not performed
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void sub(unsigned char ax, unsigned char kk) 
{
	_v[0x0f] = (_v[ax] >= kk) ? 1 : 0;
	_v[ax] -= kk;
}

/// <summary>
/// SHR Vx : 0x8XY6
/// 
/// Logical Shift Right (1)
/// 
/// Sets Vf to LSB
/// </summary>
/// <param name="vx">ax</param>
void shr(unsigned char ax)
{
	_v[0xf] = _v[ax] & 0x1;
	_v[ax] /= 2;
}

/// <summary>
/// SUBN Vx, kk
/// 
/// Subtracts Vx from kk and stores value in Vx
/// 
/// Sets Vf if borrow not performed.
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void subn(unsigned char ax, unsigned char kk)
{
	_v[0xf] = (_v[ax] <= kk) ? 1 : 0;
	_v[ax] = kk - _v[ax];
}

/// <summary>
/// SHL Vx
/// 
/// Logical Shift Left (1)
/// 
/// Sets Vf to MSB
/// </summary>
/// <param name="ax">ax</param>
void shl(unsigned char ax)
{
	_v[0xf] = _v[ax] >> 7;
	_v[ax] = _v[ax] << 1;
}



/// <summary>
/// OR Vx, kk
/// 
/// Bitwise OR on Vx and kk and store result in Vx
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void or(unsigned char ax, unsigned char kk)
{
	_v[ax] = _v[ax] | kk;
}

/// <summary>
/// AND Vx, kk
/// 
/// Bitwise AND on Vx and kk and store result in Vx
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void and(unsigned char ax, unsigned char kk)
{
	_v[ax] = _v[ax] & kk;
}

/// <summary>
/// XOR Vx, kk
/// 
/// Bitwise exclusive OR on Vx and kk and store result in Vx
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void xor(unsigned char ax, unsigned char kk)
{
	_v[ax] ^= kk;
}

/// <summary>
/// Writes a pixel to the display buffer and sets the collision flag
/// </summary>
/// <param name="ax"></param>
/// <param name="ay"></param>
/// <param name="height"></param>
void drw(unsigned char ax, unsigned char ay, unsigned char height)
{
	unsigned char pixel;
	_v[0xf] = 0x0000;
	for (int y = 0; y < height; y++)
	{
		pixel = _memory[_i + y];
		for (int x = 0; x < 8; x++)
		{
			if ((pixel & (0x80 >> x)) != 0 && ((_v[ay]+ y < 32)))
			{
				_display[(_v[ax] + x + ((_v[ay] + y) * 64))] ^= 0x0001;
				_v[0xf] = _display[_v[ax] + x + ((_v[ay] + y) * 64)] ? 0x0000 : 0x0001;
			}
		}
	}
}

/// <summary>
/// Skips next instruction is key IS pressed
/// </summary>
/// <param name="ax">ax</param>
void skp(unsigned char ax)
{
	if (_keys[ax] == 1) {
		_pc += 2;
	}
}

/// <summary>
/// Skips next instruction if key is NOT pressed
/// </summary>
/// <param name="ax">ax</param>
void sknp(unsigned char ax)
{
	if (_keys[ax] != 1) {
		_pc += 2;
	}
}

void ldi(unsigned short value)
{
	_i = value;
}

/// <summary>
/// RND Vx, kk : CXKK
/// 
/// Generates a random byte bitwise AND with kk and result stored in Vx
/// </summary>
/// <param name="ax">ax</param>
/// <param name="kk">kk</param>
void rnd(unsigned char ax, unsigned char kk)
{	
	int r = rand();      // returns a pseudo-random integer between 0 and RAND_MAX
	_v[ax] = (r & kk);
}

/// <summary>
/// Stores BCD representation of Vx in memory locations I, I+1, I+2
/// 
/// This is a direct implementation of Alexander Dicksons BCD function
/// </summary>
/// <param name="ax">ax</param>
void bcd(unsigned char ax)
{
	unsigned char number = _v[ax];
	
	unsigned int i;
	for (int i = 3; i > 0; i--)
	{
		_memory[_i + i - 1] = (number & 10);
		number /= 10;
	}
}

unsigned char *get_display()
{
	return _display;
}

unsigned char x;
unsigned char y;
void execute_instruction(unsigned short opcode) 
{
	cycle_count++;
	
	
	x = (opcode & 0x0F00) >> 8;
	y = (opcode & 0x00F0) >> 4;
	
	if (_config->verbose == 1) {
		printf("step %d / %d / pc %d / x %d / y %d / sp %d / dt %d / vy14 %d\n", cycle_count, opcode, _pc, _v[x], _v[y], _sp, _dt, _v[14]);
	}
	
	if (!_halted)
		_pc += 2;
	
	switch(opcode & 0xF000) {

			case 0x0000 :
				switch (opcode & 0x00FF)
				{
					case 0x00E0:
						cls();
					break;
					case 0x00EE:
						ret();
					break;
				}
			break;
		
		case 0x1000 :
			jp(opcode & 0x0FFF);
		break;
		case 0x2000 :
			call(opcode & 0x0FFF);
		break;
		case 0x3000 :
			se(x, opcode & 0x00FF);
		break;
		case 0x4000 :
			sne(x, opcode & 0x00FF);
		break;
		case 0x5000 :	// SE Vx, Vy
			se(x, _v[y]);
		break;
		case 0x6000 :	// LD Vx, byte
			ld(x, (opcode & 0x00FF));
		break;
		case 0x7000 :
			add(x, opcode & 0x00FF);
		break;
		case 0x8000 : 
			switch (opcode & 0x000F)
			{
				case 0x0000 :
					ld(x, _v[y]);
				break;
				case 0x0001 :
					or(x, _v[y]);
				break;
				case 0x0002 :
					and(x, _v[y]);
				break;
				case 0x0003 :
					xor(x, _v[y]);
				break;
				case 0x0004 :
					add(x, _v[y]);
				break;
				case 0x0005 :
					sub(x, _v[y]);
				break;
				case 0x0006 :
					shr(x);
				break;
				case 0x0007 :
					subn(x, _v[y]);
				break;
				case 0x000E :
					shl(x);
				break;
			}
		break;
		case 0x9000 :
			sne(x, _v[y]);
		break;
		case 0xA000 :	// LD I, addr
			ldi(opcode & 0x0FFF);
		break;
		case 0xB000 :
			jp((opcode & 0x0FFF) + _v[0]);
		break;
		case 0xC000 :	// RND Vx, byte
			rnd(x, opcode & 0x00FF);
		break;
		case 0xD000 :
			drw(x, y, opcode & 0x000F);
		break;
		case 0xE000 :
			switch (opcode & 0x00FF) {
				case 0x009E :
					skp(_v[x]);
				break;
				case 0x00A1 :
					sknp(_v[x]);
				break;
			}
		break;
		case 0xF000 :
			switch (opcode & 0x00FF) {
				case 0x0007 :
					ld(x, _dt);
				break;
				case 0x000A :
					_halted = true;
					
					for (int i = 0; i < 16; i++)
					{
						if (_keys[i] != 0)
						{
							_v[x] = i;
							_halted = false;
							continue;
						}
					}
				break;
				case 0x0015 :
					_dt = _v[x];
				break;
				case 0x0018 :
					_st = _v[x];
				break;
				case 0x001E :
					_i = _i + _v[x];
				break;
				case 0x0029 :
					_i = _v[x] * 5;
				break;
				case 0x0033 :
					bcd(x);
				break;
				case 0x0055 :
					for (int i = 0; i <= x; i++)
					{
						_memory[_i + i] = _v[i];
					}
					_i += x + 1;
				break;
				case 0x0065 :
					for (int i = 0; i <= x; i++)
					{
						ld(i, _memory[_i + i]);
					}
					_i += x + 1;
				break;
			}
		break;
	}
}
int step = 0;
unsigned short _opcode;
void cpu_cycle() {
	for (int i = 0; i < 10; i++) {
		if (!_halted) {
		_opcode = (_memory[_pc] << 8) | _memory[_pc + 1];
		}
		execute_instruction(_opcode);
		
		
	}
	
	//if (!(step++ %2)) {
		if (_dt > 0)
		{
			_dt--;
		}
		
		if (_st > 0) 
		{
			if (_st == 1) {
				S2D_PlaySound(snd1);
			}
			_st--;
		}
	//}
}

