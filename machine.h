#ifndef _MACHINE_H
#define _MACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"

// Display limits

#define X_MAX 64
#define Y_MAX 32

// To see in bigger scale

#define SCALE 5

typedef unsigned char u8;
typedef unsigned short u16;

void instruction_execute (SDL_Event *Events);

#endif
