#include "../storm.h"
#include "interpreter.h"

// execute code
void execute(std::string::iterator loc) {
	// change command executed based on value of register 0 (always 1 byte)
	switch (interpreter.registers[0][0]) {
		case 0x41:
			int fd = open(interpreter.registers[2].c_str(), O_WRONLY);

			write(fd,
				interpreter.registers[1].c_str(),
				interpreter.registers[1].size());
			
			close(fd);
			break;
	}
}