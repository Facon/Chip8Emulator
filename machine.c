/*

The MIT License

Copyright (c) 2009 Facon

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

*/

#include "machine.h"

// Memory 4 KB (4,096 bytes) of RAM

extern u8 memory[4096];

// Registers from V0 to VF

extern u8 V[16];

// Register for save 16 bits address

extern u16 I;

// Instruction Register

extern u16 IR;

// Program Counter

extern u16 PC;

// Stack, array of 16 positions, 16 bits each block

extern u16 stack[16];

// Stack Pointer

extern u16 *SP;

/*

Timers

delay timer and sound timer

delay timer

sound timer when non-zero make a beep

both runs at 60 Hz (60 cycles)

*/

extern u16 DT;
extern u16 ST;

// Display
extern u8 Display [64][32];
extern SDL_Surface *scr;

u16 BIN2BCD (u8 a, short b);
void draw_sprite(u8 x, u8 y, u8 n);
char keyboard_event(SDL_Event* keyboard);

void instruction_execute (SDL_Event *Events)
{
	u8 x;
	u8 y;
	u8 kk;
	u8 z;
	u16 i;
	u8 n;
	
	u8 xpix, yline;
	
	char key_value;
	
	switch (IR >> 12)
	{
		case 0x0:
			if (IR == 0x00E0)
			{
				/*
			
				00E0 - CLS
				Clear the display.
			
				*/
				
				PC++;
			
				for (y = 0; y < Y_MAX; y++)
					for (x = 0; x < X_MAX; x++)
					{
						Display[x][y] = 0;
					}
				
				//printf("0x00E0 - CLS\n");
			}
			else if (IR == 0x00EE)
			{
				/*
				
				00EE - RET
				Return from a subroutine.

				The interpreter sets the program counter to the address at the top of the stack,
				then subtracts 1 from the stack pointer.
				
				*/
				
				PC = *SP;
				SP--;
				//printf("0x00EE - RET\n");
			}
			else
			{
				/*
		
				0nnn - SYS addr
				Jump to a machine code routine at nnn.

				This instruction is only used on the old computers on which Chip-8 was originally implemented.
				It is ignored by modern interpreters.
				
				*/
				
				PC++;
				getchar();
				//printf("0x0nnn - SYS nnn\n");
			}
			break;
		case 0x1:
			/*
			
			1nnn - JP addr
			Jump to location nnn.

			The interpreter sets the program counter to nnn.
			
			*/
			
			PC = (IR & 0x0FFF);
			//printf("0x1%03X - JP 0x%03X\n", PC, PC);
			break;
		case 0x2:
			/*
			
			2nnn - CALL addr
			Call subroutine at nnn.

			The interpreter increments the stack pointer, then puts the current PC on the top of the stack.
			The PC is then set to nnn.
			
			*/
			
			PC++;
			SP++;
			*SP = PC;
			PC = (IR & 0x0FFF);
			//printf("0x2%03X - CALL 0x%03X\n", PC, PC);
			break;
		case 0x3:
			/*
			
			3xkk - SE Vx, byte
			Skip next instruction if Vx = kk.

			The interpreter compares register Vx to kk, and if they are equal,
			increments the program counter by 2
			
			*/
			
			PC++;
				
			x = ((IR & 0x0F00) >> 8);
			kk = (IR & 0x00FF);
			
			if (V[x] == kk)
			{
				PC += 2;
			}
			
			//printf("0x3%X%02X - SE V%X, 0x%02X\n", x, kk, x, kk);
			break;
		case 0x4:
			/*
			
			4xkk - SNE Vx, byte
			Skip next instruction if Vx != kk.

			The interpreter compares register Vx to kk, and if they are not equal,
			increments the program counter by 2.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			kk = (IR & 0x00FF);
			
			if (V[x] != kk)
			{
				PC += 2;
			}
			
			//printf("0x4%X%02X - SNE V%X, 0x%02X\n", x, kk, x, kk);
			break;
		case 0x5:
			/*
			
			5xy0 - SE Vx, Vy
			Skip next instruction if Vx = Vy.

			The interpreter compares register Vx to register Vy, and if they are equal,
			increments the program counter by 2.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			y = ((IR & 0x00F0) >> 4);
			
			if (V[x] == V[y])
			{
				PC += 2;
			}
			
			//printf("0x5%X%X0 - SE V%X, V%X\n", x, y, x, y);
			break;
		case 0x6:
			/*
			
			6xkk - LD Vx, byte
			Set Vx = kk.

			The interpreter puts the value kk into register Vx.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			kk = (IR & 0x00FF);
			
			V[x] = kk;
			
			//printf("0x6%X%02X - LD V%X, 0x%03X\n", x, kk, x, kk);
			break;
		case 0x7:
			/*
			
			7xkk - ADD Vx, byte
			Set Vx = Vx + kk.

			Adds the value kk to the value of register Vx, then stores the result in Vx.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			kk = (IR & 0x00FF);
			
			V[x] += kk;
			
			//printf("0x7%X%02X - ADD V%X, 0x%02X\n", x, kk, x, kk);
			break;
		case 0x8:
			switch (IR & 0x000F)
			{
				case 0x0:
					/*
					
					8xy0 - LD Vx, Vy
					Set Vx = Vy.

					Stores the value of register Vy in register Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[x] = V[y];
					
					//printf("0x8%X%X0 - LD V%X, V%X\n", x, y, x, y);
					break;
				case 0x1:
					/*
					
					8xy1 - OR Vx, Vy
					Set Vx = Vx OR Vy.

					Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
					A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
					then the same bit in the result is also 1. Otherwise, it is 0. 
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[x] |= V[y];
					
					//printf("0x8%X%X1 - OR V%X, V%X\n", x, y, x, y);
					break;
				case 0x2:
					/*
					
					8xy2 - AND Vx, Vy
					Set Vx = Vx AND Vy.

					Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
					A bitwise AND compares the corrseponding bits from two values, and if both bits are 1,
					then the same bit in the result is also 1. Otherwise, it is 0. 
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[x] &= V[y];
					
					//printf("0x8%X%X2 - AND V%X, V%X\n", x, y, x, y);
					break;
				case 0x3:
					/*
					
					8xy3 - XOR Vx, Vy
					Set Vx = Vx XOR Vy.

					Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
					An exclusive OR compares the corrseponding bits from two values,
					and if the bits are not both the same, then the corresponding bit in the result is set to 1.
					Otherwise, it is 0. 
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[x] ^= V[y];
					
					//printf("0x8%X%X3 - XOR V%X, V%X\n", x, y, x, y);
					break;
				case 0x4:
					/*
					
					8xy4 - ADD Vx, Vy
					Set Vx = Vx + Vy, set VF = carry.

					The values of Vx and Vy are added together.
					If the result is greater than 8 bits (i.e., > 255) VF is set to 1, otherwise 0.
					Only the lowest 8 bits of the result are kept, and stored in Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					z = V[x] + V[y];
					
					if (V[x] > V[y])
					{
						V[0xF] = (V[x] > z) ? 1 : 0;
					}
					else
					{
						V[0xF] = (V[y] > z) ? 1 : 0;
					}
					
					V[x] = z;
					
					//printf("0x8%X%X4 - ADD V%X, V%X\n", x, y, x, y);
					break;
				case 0x5:
					/*
					
					8xy5 - SUB Vx, Vy
					Set Vx = Vx - Vy, set VF = NOT borrow.

					If Vx > Vy, then VF is set to 1, otherwise 0.
					Then Vy is subtracted from Vx, and the results stored in Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[0xF] = (V[x] > V[y]) ? 1 : 0;
					
					V[x] -= V[y];
					
					//printf("0x8%X%X5 - SUB V%X, V%X\n", x, y, x, y);
					break;
				case 0x6:
					/*
					
					8xy6 - SHR Vx {, Vy}
					Set Vx = Vx SHR 1.

					If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
					Then Vx is divided by 2.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[0xF] = ((V[x] & 0x01) == 0x1) ? 1 : 0;
					V[x] >>= 1;
					
					//printf("0x8%X%X6 - SHR V%X {, V%X}\n", x, y, x, y);
					break;
				case 0x7:
					/*
					
					8xy7 - SUBN Vx, Vy
					Set Vx = Vy - Vx, set VF = NOT borrow.

					If Vy > Vx, then VF is set to 1, otherwise 0.
					Then Vx is subtracted from Vy, and the results stored in Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[0xF] = (V[y] > V[x]) ? 1 : 0;
					
					V[x] -= V[y];
					
					//printf("0x8%X%X7 - SUBN V%X, V%X\n", x, y, x, y);
					break;
				case 0xE:
					/*
					
					8xyE - SHL Vx {, Vy}
					
					Set Vx = Vx SHL 1.

					If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
					Then Vx is multiplied by 2.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					y = ((IR & 0x00F0) >> 4);
					
					V[0xF] = (V[x] & 0x80) ? 1 : 0;
					
					V[x] <<= 1;
					
					//printf("0x8%X%XE - SHL V%X {, V%X}\n", x, y, x, y);
					break;
			}
			break;
		case 0x9:
			/*
			
			9xy0 - SNE Vx, Vy
			Skip next instruction if Vx != Vy.

			The values of Vx and Vy are compared, and if they are not equal,
			the program counter is increased by 2.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			y = ((IR & 0x00F0) >> 4);
			
			if (V[x] != V[y])
			{
				PC += 2;
			}

			//printf("0x9%X%X0 - SNE V%X, V%X\n", x, y, x, y);
			break;
		case 0xA:
			/*
			
			Annn - LD I, addr
			Set I = nnn.

			The value of register I is set to nnn.
			
			*/
			
			PC++;
			
			I = (IR & 0x0FFF);	

			//printf("0xA%03X - LD I, 0x%03X\n", I, I);
			break;
		case 0xB:
			/* 

			Bnnn - JP V0, addr
			Jump to location nnn + V0.

			The program counter is set to nnn plus the value of V0.
			
			*/
			
			PC = (IR & 0x0FFF) + V[0x0];
			
			//printf("0xB%03X - JP V0, 0x%03X\n", PC, PC);
			break;
		case 0xC:
			/*
			
			Cxkk - RND Vx, byte
			Set Vx = random byte AND kk.

			The interpreter generates a random number from 0 to 255,
			which is then ANDed with the value kk. The results are stored in Vx.
			See instruction 8xy2 for more information on AND.
			
			*/
			
			PC++;
			
			x = ((IR & 0x0F00) >> 8);
			kk = (IR & 0x00FF);
			
			V[x] = (rand() & kk); // rand() % 256 is redundant
			
			//printf("0xC%X%02X - RND V%X, 0x%02X\n", x, kk, x, kk);
			break;
		case 0xD:
			/*
			
			Dxyn - DRW Vx, Vy, nibble
			Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

			The interpreter reads n bytes from memory, starting at the address stored in I.
			These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
			Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
			VF is set to 1, otherwise it is set to 0.
			If the sprite is positioned so part of it is outside the coordinates of the display,
			it wraps around to the opposite side of the screen.
			See instruction 8xy3 for more information on XOR, and section 2.4, Display,
			for more information on the Chip-8 screen and sprites.
			
			*/
			
			PC++;

			x = ((IR & 0x0F00) >> 8);
			y = ((IR & 0x00F0) >> 4);
			
			n = (IR & 0x000F);
			
			draw_sprite(x, y, n);
			
			SDL_Rect r;
			
			r.w = SCALE;
			r.h = SCALE;	
			r.y = (V[y]*SCALE);
			r.x = (V[x]*SCALE);
			
			for (yline = 0; (yline < n); yline++)
				for(xpix = 0; (xpix < 8); i++)
				{SDL_FillRect(scr, &r, (Display[V[x]+xpix][V[y]+xpix] == 1) ? 0xFF : 0x0);}
			//SDL_FillRect(scr, &r, (Display[V[x]][V[y]] == 1) ? 0xFF : 0x0);
			SDL_UpdateRect(scr, r.x, r.y, r.w*8, r.h*n);
			
			//printf("0xD%X%X%X - DRW V%X, V%X, 0x%X\n", x, y, n, x, y, n);
			break;
		case 0xE:
			switch(IR & 0x00FF)
			{
				case 0x9E:
					/*
					
					Ex9E - SKP Vx
					Skip next instruction if key with the value of Vx is pressed.

					Checks the keyboard,
					and if the key corresponding to the value of Vx is currently in the down position,
					PC is increased by 2.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					
					key_value = keyboard_event(Events);
					
					if (key_value == V[x])
					{
						PC += 2;
					}
					
					//printf("0xE%X9E - SKP V%X\n", x, x);
					break;
				case 0xA1:
					/*
					
					ExA1 - SKNP Vx
					Skip next instruction if key with the value of Vx is not pressed.

					Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					
					key_value = keyboard_event(Events);
					
					if (key_value != V[x])
					{
						PC += 2;
					}
					
					//printf("0xE%XA1 - SKNP V%X\n", x, x);
					break;			
			}
			break;
		case 0xF:
			switch(IR & 0x00FF)
			{
				case 0x07:
					/*
					
					Fx07 - LD Vx, DT
					Set Vx = delay timer value.

					The value of DT is placed into Vx.
					
					*/
					
					PC++;
					
					x = (IR & 0x0F00) >> 8;
					V[x] = DT;
					
					//printf("0xF%X07 - LD V%X, DT = 0x%X\n", x, x, DT);
					break;
				case 0x0A:
					/*
					
					Fx0A - LD Vx, K
					Wait for a key press, store the value of the key in Vx.

					All execution stops until a key is pressed, then the value of that key is stored in Vx.
					
					*/
					
					PC++;
					
					x = (IR & 0x0F00) >> 8;
					
					key_value = -1;
					
					while (key_value == -1)
					{
						key_value = keyboard_event(Events);
					}
					
					V[x] = key_value;
										
					//printf("0xF%X0A - LD V%X, DT = 0x%X\n", x, x, DT);
					break;
				case 0x15:
					/*
					
					Fx15 - LD DT, Vx
					Set delay timer = Vx.

					DT is set equal to the value of Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					DT = V[x];
					
					//printf("0xF%X15 - LD DT = 0x%X, V%X\n", x, DT, x);
					break;
				case 0x18:
					/*
					
					Fx18 - LD ST, Vx
					Set sound timer = Vx.

					ST is set equal to the value of Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					ST = V[x];
					
					//printf("0xF%X18 - LD ST = 0x%X, V%X\n", x, ST, x);
					break;
				case 0x1E:
					/*
					
					Fx1E - ADD I, Vx
					Set I = I + Vx.

					The values of I and Vx are added, and the results are stored in I.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					I += V[x];
					
					//printf("0xF%X1E - ADD I, V%X\n", x, x);
					break;
				case 0x29:
					/*
 
					Fx29 - LD F, Vx
					Set I = location of sprite for digit Vx.

					The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
					See section 2.4, Display, for more information on the Chip-8 hexadecimal font. 

					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);					
					
					// 5 Bytes representation per number
					
					I = (5 * V[x]);

					//printf("0xF%X29 - LD F, V%X\n", x, x);
					break;
				case 0x33:
					/*
					
					Fx33 - LD B, Vx
					Store BCD representation of Vx in memory locations I, I+1, and I+2.

					The interpreter takes the decimal value of Vx,
					and places the hundreds digit in memory at location in I,
					the tens digit at location I+1, and the ones digit at location I+2.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					
					// Revisar porque puede estar mal
					
					memory[I] = BIN2BCD(V[x], 3);
					memory[I+1] = BIN2BCD(V[x], 2);
					memory[I+2] = BIN2BCD(V[x], 1);
					
					//printf("0xF%X33 - LD B, V%X\n", x, x);
					break;
				case 0x55:
					/*
					
					Fx55 - LD [I], Vx
					Store registers V0 through Vx in memory starting at location I.

					The interpreter copies the values of registers V0 through Vx into memory,
					starting at the address in I.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					for (i = 0; (i <= x); i++)
					{
						memory[I++] = V[i];
					}
					
					//printf("0xF%X55 - LD [I], V%X\n", x, x);
					break;
				case 0x65:
					/*
					
					Fx65 - LD Vx, [I]
					Read registers V0 through Vx from memory starting at location I.

					The interpreter reads values from memory starting at location I into registers V0 through Vx.
					
					*/
					
					PC++;
					
					x = ((IR & 0x0F00) >> 8);
					for(i = 0; (i <= x); i++)
					{
						V[i] = memory[I++];
					}
					
					//printf("0xF%X65 - LD I, V[%X]\n", x, x);
					break;
			}
			break;
		default:
			printf("Unknown OPCODE.\n");
			printf("0x%03x - ??\n", IR);
			exit(1);
			break;
	}		
}

u16 BIN2BCD (u8 a, short b)
{
	switch(b)
	{
		case 1:
			return a%10;
			break;
		case 2:
			return (a%100)/10;
			break;
		case 3:
			return a/100;
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

void draw_sprite(u8 x, u8 y, u8 n)
{
	// Draw to Display
	
	u16 xpixel, yline;
	u8 data;
	
	V[0xF] = 0;
	
	for(yline = 0; (yline < n); yline++)
	{
		data = memory[I + yline];
		for(xpixel = 0; (xpixel < 8); xpixel++)
		{
			if((data & (0x80 >> xpixel)) != 0)
			{
				if (Display[V[x] + xpixel][V[y] + yline] == 1)
				{
					V[0xF] = 1;
				}
				Display[V[x] + xpixel][V[y] + yline] ^= 1;
			}
		}
	}
}

char keyboard_event(SDL_Event* keyboard)
{
/*

KEYBOARD CHIP-8

_________
|1|2|3|C|
---------
|4|5|6|D|
---------
|7|8|9|E|
---------
|A|0|B|F|
---------

*/
	while (SDL_PollEvent(keyboard))
	{
		switch (keyboard -> type)
		{
			case SDL_KEYDOWN:
				switch (keyboard -> key.keysym.sym)
				{
					case SDLK_1: // 1
						//getchar();
						return 0x1;
						break;
					case SDLK_2: // 2
						//getchar();
						return 0x2;
						break;
					case SDLK_3: // 3
						//getchar();
						return 0x3;
						break;
					case SDLK_4: // C
						//getchar();
						return 0xC;
						break;
					case SDLK_q: // 4
						//getchar();
						return 0x4;
						break;
					case SDLK_w: // 5
						//getchar();
						return 0x5;
						break;
					case SDLK_e: // 6
						//getchar();
						return 0x6;
						break;
					case SDLK_r: // D
						//getchar();
						return 0xD;
						break;
					case SDLK_a: // 7
						//getchar();
						return 0x7;
						break;
					case SDLK_s: // 8
						//getchar();
						return 0x8;
						break;
					case SDLK_d: // 9
						//getchar();
						return 0x9;
						break;
					case SDLK_f: // E
						//getchar();
						return 0xE;
						break;
					case SDLK_z: // A
						//getchar();
						return 0xA;
						break;
					case SDLK_x: // 0
						//getchar();
						return 0x0;
						break;
					case SDLK_c: // B
						//getchar();
						return 0xB;
						break;
					case SDLK_v: // F
						//getchar();
						return 0xF;
						break;
					default:
						return -1;
				}
				break;
		}
	}
	return -1;
}
