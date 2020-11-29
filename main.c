/*

Chip-8 Emulator - Pinocho

The MIT License

Copyright (c) 2009 Facon

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "machine.h"

// Memory 4 KB (4,096 bytes) of RAM

u8 memory[4096];

// Registers from V0 to VF

u8 V[16];

// Register for save 16 bits address

u16 I = 0;

// Instruction Register

u16 IR;

// Program Counter

u16 PC;

// Stack, array of 16 positions, 16 bits each block

u16 stack[16];

// Stack Pointer

u16 *SP;

/*

Timers

delay timer and sound timer

delay timer

sound timer when non-zero make a beep

both runs at 60 Hz (60 cycles)

*/

u16 DT;
u16 ST;

// Clock of 60 Hz

#define CLOCK 60

u8 cycles = CLOCK;

// Display

u8 Display [X_MAX][Y_MAX];
SDL_Surface *scr;

SDL_Surface *init_SDL();
void load_rom();
void load_game(char *game_name);

int main(int argv, char *argc[])
{
	PC = 0x200;
	SP = &stack[0];
	DT = 0;
	ST = 0;

	scr = init_SDL();
	SDL_Event Events;

	// Getting pseudo-random numbers
	srand (time(NULL));
	// Loading ROM in memory
	load_rom();
	// Loading game in memory
    if (argv > 1)
    {
        load_game(argc[1]);
    }
    else
    {
        printf("No Game!\n");
        return 0;
    }
    
    unsigned char running = 1;

	while (running == 1)
	{
		// fetch
		IR = memory[PC++];
		IR = ((IR << 8) | memory[PC]);
		// Decreasing cycle of clock
		cycles--;
		// Decode and execution
		instruction_execute (&Events);
		// Sound maker :P
		if (DT != 0)
		{
			//printf("\7");
		}
		if (cycles <= 0)
		{
			DT--;
			ST--;
			cycles = CLOCK;
		}

		while (SDL_PollEvent(&Events))
		{
			switch(Events.type)
			{
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch (Events.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = 0;
							break;
						default:
							break;
					}
					break;
			}
		}
	}

	return 0;
}

SDL_Surface* init_SDL()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen;
	screen = SDL_SetVideoMode((X_MAX * SCALE), (Y_MAX * SCALE), 8, SDL_SWSURFACE);
	SDL_WM_SetCaption("Another chip-8 emulator", 0);
	SDL_Color palette[] =
	{
		{0, 0, 0, 0},
		{255, 255, 255, 255}
	};
	SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, palette, 0, 2);
	return screen;
}

void load_rom()
{
	FILE *rom;
	rom = fopen("CHIP8.ROM", "r");
	if (rom == NULL)
	{
		printf("Error, no encontrado \"CHIP8.ROM\"n.");
		exit(1);
	}
	else
	{
		short cont_chars = 0;
		while(!feof(rom))
		{
			memory[cont_chars] = fgetc(rom);
			cont_chars++;
		}
	}
	fclose(rom);
}

void load_game(char *game_name)
{
	FILE *game;
	printf("LOADING GAME %s\n", game_name);
	game = fopen(game_name, "r");
	if (game == NULL)
	{
		printf("Error, not found %s.\n", game_name);
		exit(1);
	}
	else
	{
		short cont_bytes = 0;
		while(!feof(game))
		{
			// Put game in memory
			memory[cont_bytes+0x200] = fgetc(game);
			cont_bytes++;
		}
	}
	fclose(game);
}
