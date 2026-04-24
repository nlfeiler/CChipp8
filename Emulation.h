#pragma once

#include "Chip8.h"

int emulate(Chip8 &chip8);
void fetch(Chip8 &chip8);
int execute();