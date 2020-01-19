#include "../storm.h"
#include "interpreter.h"

// move bytecode contents to vector
void moveToVector(std::string contents) {
	for (char byte : contents)
		program.bytecode.push_back(uint8_t(byte));
}

void interpret(std::string contents) {
	moveToVector(contents);

	for (auto byte = program.bytecode.begin(); byte != program.bytecode.end(); byte++) {
		switch (*byte)
		{
			case 0x0A: // write
				write(byte);
				break;
			
			default:
				break;
		}
	}
}