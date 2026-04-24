#include "Chip8.h"
#include "Emulation.h"

#include <iostream>
#include <fstream>
#include <algorithm>

int main(int argc, char** argv)
{
	//print out all of our command line args this will be used later on
	for (char** i = argv; i < argv + argc; i++) std::cout << *i << std::endl;

	Chip8 chip8; //create our chip8 machine struct here that we will be using for the whole emulator

	//now we will find our rom to read and make sure it exists
	std::ifstream file(argv[1], std::ios::binary);
	if(!file){
		std::cout << "File not found!" << std::endl;
		return -1;
	}
	std::cout << "Found file!" << std::endl;

	//TODO: move the rom reading into memory into Emulation.cpp so I can save a bit of memory instead of having two different Chip8 structs in memory
	std::fill(std::begin(chip8.ram), std::end(chip8.ram), 0x00);

	//now we will read our rom into memory
	char byte; //this will serve as the current byte in the rom we are reading
	int ramLocation = 0x200; //this will serve as the location in the memory that we are going to be placing the byte into
	while (file.get(byte)) {
		chip8.ram[ramLocation] = byte;
		ramLocation++;
	}

	emulate(chip8);

	//once the above has been completed we can actuall now step into our emulation cycle

	return 0;
}