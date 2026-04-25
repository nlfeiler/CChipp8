#include "Chip8.h"
#include "Graphics.h"

#include <algorithm>
#include <iostream>
#include <array>
#include <bitset>
#include <chrono>
#include <thread>

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
			if(byteTwo == 0xE0){ //case 0x00E0
				std::cout << "Clearing screen..." << std::endl;
				for (int x = 0; x < 64; x++) for (int y = 0; y < 32; y++) chip8.screen[x][y] = 0; //fill our boolean screen array with 0s to signify each pixel as "off"
			}
			else if (byteTwo == 0xEE) { //opcode 0x00EE
				std::cout << "Returning from subroutine..." << std::endl;
				chip8.sp--; //decrement the stack pointer
				chip8.pc = chip8.stack[chip8.sp]; //set the pc equal to the value at the stack pointer
			}
			break;
		case 0x1:
			std::cout << "Jumping to subroutine..." << std::endl;
			chip8.pc = opcode & 0x0FFF; //set our pc equal to NNN
			pcIncrement = 0; //make sure we do not increment the pc

			if (chip8.pc == (opcode & 0x0FFF) && chip8.quitOnLoop){ //infinite loop detection
				pcIncrement = -1;
				std::cout << "Infinte loop detected. Halting..." << std::endl;
			}

			break;
		case 0x2:
			std::cout << "Calling subroutine..." << std::endl;

			chip8.stack[chip8.sp] = chip8.pc;
			chip8.sp++;
			chip8.pc = opcode & 0x0FFF;
			pcIncrement = 0;

			break;
		case 0x3:
			std::cout << "Checking Vx == NN..." << std::endl;

			if (chip8.registers[lowNibbleByteOne] == byteTwo) {
				std::cout << "Equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4;
			}else std::cout << "Not equal..." << std::endl;

			break;
		case 0x4:
			std::cout << "Checking Vx != NN..." << std::endl;
			if (chip8.registers[lowNibbleByteOne] != byteTwo) {
				std::cout << "Not equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4;
			}else std::cout << "Equal..." << std::endl;

			break;

		case 0x5:
			std::cout << "Checking Vx == Vy..." << std::endl;

			if (chip8.registers[lowNibbleByteOne] == chip8.registers[highNibbleByteTwo]) { //check vx == vy
				std::cout << "Equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4; //if equal we need to skip the next instruction
			}
			else std::cout << "Not equal..." << std::endl;

			break;
		case 0x6:
			std::cout << "Changing register value..." << std::endl;
			chip8.registers[lowNibbleByteOne] = byteTwo; //set the register value to NN
			break;
		case 0x7:
			std::cout << "Adding NN to Vx..." << std::endl;
			chip8.registers[lowNibbleByteOne] += byteTwo; //add NN to the register specified
			break;
		case 0x8:
			switch (lowNibbleByteTwo) { //check the low nibble of byte two to check what opcode we are dealing with
				case 0x0:
					std::cout << "Setting Vx = Vy..." << std::endl;
					chip8.registers[lowNibbleByteOne] = chip8.registers[highNibbleByteTwo]; //set Vx equal to the value in Vy
					break;
				case 0x1:
					std::cout << "Setting Vx to Vx | Vy..." << std::endl;
					chip8.registers[lowNibbleByteOne] |= chip8.registers[highNibbleByteTwo];
					break;
				case 0x2:
					std::cout << "Setting Vx to Vx & Vy..." << std::endl;
					chip8.registers[lowNibbleByteOne] &= chip8.registers[highNibbleByteTwo];
					break;
				case 0x3:
					std::cout << "Setting Vx to Vx ^ Vy..." << std::endl;
					chip8.registers[lowNibbleByteOne] ^= chip8.registers[highNibbleByteTwo];
					break;
				case 0x4:
					std::cout << "Vx += Vy..." << std::endl;
					chip8.registers[0xF] = 0;
					chip8.registers[lowNibbleByteOne] += chip8.registers[highNibbleByteTwo];
					if (chip8.registers[lowNibbleByteOne] + chip8.registers[highNibbleByteTwo] > 255) {
						chip8.registers[0xF] = 1;
						chip8.registers[lowNibbleByteOne] = (chip8.registers[lowNibbleByteOne] + chip8.registers[highNibbleByteTwo] - 255);
					}
					break;
				case 0x5:
					std::cout << "Vx -= Vy..." << std::endl;
					chip8.registers[0xF] = 0;
					chip8.registers[lowNibbleByteOne] -= chip8.registers[highNibbleByteTwo];
					if(chip8.registers[highNibbleByteTwo] >= chip8.registers[lowNibbleByteOne]) chip8.registers[0xF] = 1;
					break;
				case 0x6:
					std::cout << "Vx >>= 1..." << std::endl;
					chip8.registers[0xF] = chip8.registers[lowNibbleByteOne] >> 7;
					chip8.registers[lowNibbleByteOne] >>= 1;
					break;
				case 0x7:
					std::cout << "Setting Vx to Vy - Vx..." << std::endl;
					chip8.registers[0xF] = 0;
					chip8.registers[lowNibbleByteOne] = chip8.registers[highNibbleByteTwo] - chip8.registers[lowNibbleByteOne];
					if (chip8.registers[highNibbleByteTwo] >= chip8.registers[lowNibbleByteOne]) chip8.registers[0xF] = 1;
					break;
				case 0xE:
					std::cout << "Vx <<= 1..." << std::endl;
					chip8.registers[0xF] = chip8.registers[lowNibbleByteOne] << 7;
					chip8.registers[lowNibbleByteOne] <<= 1;
					break;
				default:
					std::cout << "Unsupported opcode " << std::hex << byteOne << std::hex << byteTwo << std::endl; //unsupported opcode
					break;
			}
			break;
		case 0x9:
			std::cout << "Checking Vx != Vy" << std::endl;

			if (chip8.registers[lowNibbleByteOne] != chip8.registers[highNibbleByteTwo]) {//check vx!=vy
				std::cout << "Not Equal. Skipping next instruction..." << std::endl;
				pcIncrement = 4; //if they are not equal we need to skip the next instruction
			}
			else std::cout << "Equal..." << std::endl; //do not skip the next instruction

			break;
		case 0xA:
			std::cout << "Setting I to NNN..." << std::endl;
			chip8.I = opcode & 0x0FFF; //set Index register equal to NNN
			break;
		case 0xB:
			std::cout << "Setting PC to V0 + NNN..." << std::endl;
			chip8.pc = chip8.registers[0] + (opcode & 0x0FFF); //set pc equal to register 0 plus NNN
			break;
		case 0xD:
			std::cout << "Drawing Sprite to Screen at Vx, Vy with height of N..." << std::endl;

			if(1){
				int vx = chip8.registers[lowNibbleByteOne]; //get the datum in register vx to use as x coord
				int vy = chip8.registers[highNibbleByteTwo]; //get the datum in register vy to use as y coord

				chip8.registers[0xF] = 0; //set register F to 0 
				for (int row = 0; row < lowNibbleByteTwo; row++) {
					int spriteByte = chip8.ram[chip8.I + row]; //set the sprite we will be creating to Index register I plus the row we are in
					for (int col = 0; col < 8; col++){
						int spriteBit = (spriteByte >> (7 - col)) & 1; //get the bit for this specific pixel we are drawing

						if(spriteBit == 1 && chip8.screen[vx + col][vy + row] == 1) chip8.registers[0xF] = 1; //if the pixel is already on then we set register F to 1 to show collision

						chip8.screen[vx + col][vy + row] = spriteBit; //now we actually set the screen's pixel to on
					}
				}
			}
			
			break;
		case 0xF:
			switch (byteTwo) {
				case 0x1E:
					std::cout << "I += Vx" << std::endl;
					chip8.I += chip8.registers[lowNibbleByteOne];
					break;
				case 0x33:
					std::cout << "Storing Vx in ram..." << std::endl;
					chip8.ram[chip8.I] = chip8.registers[lowNibbleByteOne] / 100;
					chip8.ram[chip8.I + 1] = (chip8.registers[lowNibbleByteOne] / 10) % 10;
					chip8.ram[chip8.I + 2] = chip8.registers[lowNibbleByteOne] % 10;
					break;
				case 0x55:
					std::cout << "Dumping registers to ram..." << std::endl;
					for (int i = 0; i <= lowNibbleByteOne; i++) {
						chip8.ram[chip8.I + i] = chip8.registers[i];
					}
					break;
				case 0x65:
					std::cout << "Filling Vx with values from m[I]..." << std::endl;

					for (int n = 0; n < lowNibbleByteTwo; n++) {
						chip8.registers[n] = chip8.ram[chip8.I + n];
					}
					break;
				default:
					std::cout << "Unsupported opcode " << std::hex << byteOne << std::hex << byteTwo << std::endl; //unsupported opcode
					break;
			}
			break;
		default:
			std::cout << "Unsupported opcode " << std::hex << byteOne << std::hex << byteTwo << std::endl;
			pcIncrement = 0;
			return -1;
	}
	
	return pcIncrement;
}

//this simply fetches the next opcode as its four requisite nibbles and then returns those nibbles in an array
std::array<int, 4> fetch(Chip8 &chip8) {
	int highNibbleByteOne = chip8.ram[chip8.pc] >> 4; //get the high nibble of the first byte
	int lowNibbleByteOne = chip8.ram[chip8.pc] & 0x0F; //get the low nibble of the first byte

	int highNibbleByteTwo = chip8.ram[chip8.pc+1] >> 4; //get the high nibble of the second byte
	int lowNibbleByteTwo = chip8.ram[chip8.pc+1] & 0x0F; //get the low nibble of the second byte

	return {highNibbleByteOne, lowNibbleByteOne, highNibbleByteTwo, lowNibbleByteTwo}; //return our nibbles
}

//this does all the important stuff for our emulator such as getting the opcode, executing the instructions then incrementing pc and updating the screen
void cycle(Chip8 &chip8) {
	auto nibbles = fetch(chip8); //get the nibbles of the next chip8 opcode
	int pcIncrement = 0; //stores how much the pc needs to be incremented by
	pcIncrement = execute(chip8, nibbles); //execute our emulation of the fetched nibbles and then increment pc by the requisite amount
	if (pcIncrement == -1) isRunning = false; //this is here strictly just for exiting the program when an infinite loop is detected
	else chip8.pc += pcIncrement; //increment our pc otherwise if no loop
	drawScreen(chip8); //now we need to update our screen
}

//this function should only run once and that should be when the program has started
void initSystem(Chip8 &chip8) {
	std::fill(std::begin(chip8.registers), std::end(chip8.registers), 0x0); //fill the registers with an initial value of 0
	for (int x = 0; x < 64; x++) for (int y = 0; y < 32; y++) chip8.screen[x][y] = 0; //fill our boolean screen array with 0s to signify each pixel as "off"
	std::fill(std::begin(chip8.stack), std::end(chip8.stack), 0x0); //fill the stack to initial values of 0

	chip8.pc = 0x200; //start our program counter at 0x0200 since most chip8 interpreters have the interpreter occupy 0x0 through 0x200 in memory
	//load the fontset into ram now
	for(int i = 0x50; i < 0x50 + sizeof(chip8.font)/sizeof(int); i++) chip8.ram[i] = chip8.font[i];

	initWindow();
}

int emulate(Chip8 &chip8) {
	initSystem(chip8); //initialize the system to get it ready for emulation

	const std::chrono::duration<double> cycleTime(1.0 / chip8.cpuFrequency); //create a duration variable of the cpu's period

	//just do our cycles while the system is running, this also handles timing
	while (isRunning) {
		auto startTime = std::chrono::high_resolution_clock::now(); //get the current time and store it
		
		cycle(chip8); //do our emulator cycle

		auto endTime = std::chrono::high_resolution_clock::now(); //now we get the time at the end of the cycle and store it
		std::chrono::duration<double> timeElapsed = endTime - startTime; //now we get the time that has been elapsed in our cycle
		if((timeElapsed < cycleTime) && !chip8.noHz) std::this_thread::sleep_for(cycleTime - timeElapsed); //if the amount of time it took for the cycle was not accurate to real hardware timing then sleep for the needed remaining amount of time
	}

	return 0;
}