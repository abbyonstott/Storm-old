/*
    interpreter.cc - General interpreter functions
    Copyright (C) 2020 Ethan Onstott

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../Storm/storm.h"
#include "interpreter.h"
#include "../file/file.h"

// mov value to register
void move(std::string::iterator loc) {
	loc++;
	
	// determine register
	int reg = *loc - 0x0F;

	loc++;
	
	// load value from next bytecode
	switch (*loc) {
		case 0x40: // read
		case 0x41: // write
			interpreter.registers[reg] = *loc;
			loc++;
			break;
		case char('[' + 0x80):
			interpreter.registers[reg] = getVal(loc);

			if (interpreter.registers[reg][0] == '\"') 
				stripString(&interpreter.registers[reg]);
			
			break;
	}
	
}

void execute(std::string::iterator loc) {
	// change command executed based on value of register 0 (always 1 byte)
	switch (interpreter.registers[0][0]) {
		case 0x40: // read
		{
			int fd = open(interpreter.registers[1].c_str(), O_RDONLY);
			char buf[std::stoi(interpreter.registers[2])];
			
			read(
				fd,
				buf,
				std::stoi(interpreter.registers[2])
			);				
			
			interpreter.stack.push(buf);

			// c strings append octal escapes that mess up the values, this will remove them
			while (interpreter.stack.top().size() > std::stoi(interpreter.registers[2])) 
				interpreter.stack.top().pop_back();

			interpreter.stack.top().insert(interpreter.stack.top().begin(), '"');
			interpreter.stack.top().push_back('"');

			close(fd);

			break;
		}
		case 0x41: // write
		{	
			int fd = open(interpreter.registers[2].c_str(), O_WRONLY);

			write(fd,
				interpreter.registers[1].c_str(),
				interpreter.registers[1].size());
			
			close(fd);
			
			break;
		}
	}
}

void runScope(std::string::iterator &startloc) {
	bool endScope = false;

	for (auto loc = startloc; loc != interpreter.contents.end(); loc++) {
		switch (*loc) {
			case 0x0A: // exec
				execute(loc);
				break;
			case 0x0E: // move
				move(loc);
				break;
			case 0x1A: // exit
				exit(0);
			case 0x19: // ret
				endScope = true;
				break;
			case 0x18: // pop
				popStack(loc);
				break;
			case 0x1C: // call
				callFunc(loc);
				break;
		}

		if (endScope) break;
	}
}

int main(int argc, char *argv[]) {
	program.filename = argv[1];
	interpreter.contents = readFile();
	std::string::iterator loc = interpreter.contents.begin();

	allocateMemory(loc);
	createFunctions(loc);
	runScope(loc);

	return EXIT_SUCCESS;
}