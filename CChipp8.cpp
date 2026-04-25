#include "Chip8.h"
#include "Emulation.h"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
	Chip8 chip8; //create our chip8 machine struct here that we will be using for the whole emulator

	int fileArgvIndex = 0;

	//check each command line arg and do what needs to be done with it
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-f") { //this arg sets what file we are to emulate
			std::cout << "File " << argv[i+1] << std::endl;
			fileArgvIndex = i+1;
		}
		else if (arg == "-hz") { //this arg sets the cpu hertz/frequency
			std::cout << "Setting CPU Hz to " << argv[i+1] << std::endl;
			chip8.cpuFrequency = std::stoi(argv[i+1]);
			chip8.cpuPeriod = 1.0 / chip8.cpuFrequency;
		}
		else if (arg == "-lq") { //this flag is for quitting upon an infinite loop
			std::cout << "Quit on loop" << std::endl;
			chip8.quitOnLoop = true;
		}
		else if (arg == "-nhz") {

		}
	}

	//now we will find our rom to read and make sure it exists
	std::ifstream file(argv[fileArgvIndex], std::ios::binary);
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
		chip8.ram[ramLocation] = byte; //read the current byte into the chip8's ram
		ramLocation++; //increment our location
	}

	emulate(chip8); //do the emulation now

	return 0;
}