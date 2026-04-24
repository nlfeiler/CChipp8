#include "Chip8.h"
#include "Graphics.h"

#include <algorithm>
#include <iostream>
#include <array>
#include <bitset>

bool isRunning = true;

int execute(Chip8 &chip8, std::array<int, 4> nibbles) {
	int pcIncrement = 2;

	int highNibbleByteOne = nibbles[0];
	int lowNibbleByteOne = nibbles[1];
	int highNibbleByteTwo = nibbles[2];
	int lowNibbleByteTwo = nibbles[3];

	int byteOne = (highNibbleByteOne << 4) | lowNibbleByteOne;
	int byteTwo = (highNibbleByteTwo << 4) | lowNibbleByteTwo;
	uint16_t opcode = (byteOne << 8) | byteTwo;
	//std::cout << std::hex << byteOne;
	//std::cout << std::hex << byteTwo << std::endl;

	switch (highNibbleByteOne) {
		case 0x0:
			if(byteTwo == 0xE0){
				std::cout << "Clearing screen..." << std::endl;
				for (int x = 0; x < 64; x++) for (int y = 0; y < 32; y++) chip8.screen[x][y] = 0; //fill our boolean screen array with 0s to signify each pixel as "off"
			}
			else if (byteTwo == 0xEE) {
				std::cout << "Returning from subroutine..." << std::endl;
				chip8.sp--;
				chip8.pc = chip8.stack[chip8.sp];
			}
			break;
		case 0x1:
			std::cout << "Jumping to subroutine..." << std::endl;
			chip8.pc = opcode & 0x0FFF;
			pcIncrement = 0;
			break;
		case 0x5:
			std::cout << "Checking Vx == Vy" << std::endl;
			if (chip8.registers[lowNibbleByteOne] == chip8.registers[highNibbleByteTwo]) {
				std::cout << "Equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4;
			}
			else {
				std::cout << "Not equal..." << std::endl;
			}
			break;
		case 0x6:
			std::cout << "Changing register value..." << std::endl;
			chip8.registers[lowNibbleByteOne] = byteTwo;
			break;
		case 0x7:
			std::cout << "Adding NN to Vx" << std::endl;
			chip8.registers[lowNibbleByteOne] += byteTwo;
			break;
		case 0x8:
			switch (lowNibbleByteTwo) {
				case 0x0:
					std::cout << "Setting Vx = Vy" << std::endl;
					chip8.registers[lowNibbleByteOne] = chip8.registers[highNibbleByteTwo];
					break;
				default:
					std::cout << "Unsupported opcode " << std::hex << byteOne << std::hex << byteTwo << std::endl;
			}
			break;
		case 0x9:
			std::cout << "Checking Vx != Vy" << std::endl;
			if (chip8.registers[lowNibbleByteOne] != chip8.registers[highNibbleByteTwo]) {
				std::cout << "Not Equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4;
			}
			else {
				std::cout << "Equal..." << std::endl;
			}
			break;
		case 0xA:
			std::cout << "Setting I to NNN" << std::endl;
			chip8.I = opcode & 0x0FFF;
			break;
		case 0xB:
			std::cout << "Setting PC to V0 + NNN" << std::endl;
			chip8.pc = chip8.registers[0] + (opcode & 0x0FFF);
			break;
		case 0xD:
			std::cout << "Drawing Sprite to Screen at Vx, Vy with height of N" << std::endl;

			if(1){
				int vx = chip8.registers[lowNibbleByteOne];
				int vy = chip8.registers[highNibbleByteTwo];

				chip8.registers[0xF] = 0;
				for (int row = 0; row < lowNibbleByteTwo; row++) {
					int spriteByte = chip8.ram[chip8.I + row];
					for (int col = 0; col < 8; col++){
						int spriteBit = (spriteByte >> (7 - col)) & 1;

						if(spriteBit == 1 && chip8.screen[vx + col][vy + row] == 1) chip8.registers[0xF] = 1;

						chip8.screen[vx + col][vy + row] = spriteBit;
					}
				}
			}
			
			break;
		default:
			std::cout << "Unsupported opcode " << std::hex << byteOne << std::hex << byteTwo << std::endl;
			pcIncrement = 0;
			return -1;
	}
	
	return pcIncrement;
}

std::array<int, 4> fetch(Chip8 &chip8) {
	int highNibbleByteOne = chip8.ram[chip8.pc] >> 4;
	int lowNibbleByteOne = chip8.ram[chip8.pc] & 0x0F;

	int highNibbleByteTwo = chip8.ram[chip8.pc+1] >> 4;
	int lowNibbleByteTwo = chip8.ram[chip8.pc+1] & 0x0F;

	//std::cout << std::hex << highNibbleByteOne << " " << std::hex << lowNibbleByteOne << std::endl;
	return {highNibbleByteOne, lowNibbleByteOne, highNibbleByteTwo, lowNibbleByteTwo};
}

void cycle(Chip8 &chip8) {
	auto nibbles = fetch(chip8);
	int pcIncrement = 0;
	pcIncrement = execute(chip8, nibbles);
	if (pcIncrement == -1)
		isRunning = false;
	else chip8.pc += pcIncrement;
	drawScreen(chip8);
}

//this function should only run once and that should be when the program has started
void initSystem(Chip8 &chip8) {
	std::fill(std::begin(chip8.registers), std::end(chip8.registers), 0x0); //fill the registers with an initial value of 0
	for (int x = 0; x < 64; x++) for (int y = 0; y < 32; y++) chip8.screen[x][y] = 0; //fill our boolean screen array with 0s to signify each pixel as "off"
	std::fill(std::begin(chip8.stack), std::end(chip8.stack), 0x0); //fill the stack to initial values of 0

	chip8.pc = 0x200; //start our program counter at 0x0200 since most chip8 interpreters have the interpreter occupy 0x0 through 0x200 in memory
	//load the fontset into ram now
	for(int i = 0x50; i < 0x50 + sizeof(chip8.font)/sizeof(int); i++)
		chip8.ram[i] = chip8.font[i];

	initWindow();
}

int emulate(Chip8 &chip8) {
	initSystem(chip8);

	while (isRunning) {
		cycle(chip8);
	}

	return 0;
}