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

// get location of var in heap
int getHeapLoc(std::string::iterator &loc) {
	loc++;
	std::vector<int> num_list;
	int num = 0;

	while (char(*loc - 0x80) != ']') {
		// get integer representation of digit in char
		num_list.push_back((int)(char(*loc - 0x80) - '0'));
		loc++;
	}

	for (int n = 0; n < num_list.size(); n++)
		num += num_list[n] * (n + 1);

	return num;
}

std::string getVal(std::string::iterator &loc) {
	std::string value;
	
	int num = getHeapLoc(loc);
	int i = interpreter.heap_ptrs[num];

	if (interpreter.heap[i] == 0x15) {
		std::cerr << "Error: tried to access unitialized variable at address " << num << '\n';
		exit(1);
	}

	while(i < interpreter.heap_ptrs[num + 1]) {
		value += ((interpreter.heap[interpreter.heap_ptrs[num]] > 0x7f) 
			? interpreter.heap[i] - 0x80 
			: interpreter.heap[i] + '0'
		);
		i++;
	}

	return value;
}

void allocateMemory(std::string::iterator &loc) { // load data into memory
	loc++;

	while (*loc != 0x0B) {
		if (char(*loc - 0x80) == '[') {
			while (char(*loc - 0x80) != ']') loc++;
			loc++;
			interpreter.heap_ptrs.push_back(interpreter.heap.size());
		}

		// not unitialized
		if (*loc != 0x15) {
			// variable defined to be other variable
			if (char(*loc - 0x80) == '[') {
				loc++;
				for (char c : getVal(loc))
					interpreter.heap.push_back(c + 0x80);
			}
			else { // var defined as literal
				do {
					loc++;
					interpreter.heap.push_back(*loc);
				} while (char(*(loc + 1) - 0x80) != '[' && *(loc +1) != 0x0B);
			}
		}
		else {
			interpreter.heap.push_back(0x15);
		}

		loc++;
	}

	interpreter.heap_ptrs.push_back(interpreter.heap.size());
}

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

void popStack(std::string::iterator &loc) {
	loc++;
	int num = getHeapLoc(loc);
	std::string stop = interpreter.stack.top(); // value of top
	int sval_size = stop.size();
	int orig_size = interpreter.heap_ptrs[num + 1] - interpreter.heap_ptrs[num];
	
	interpreter.stack.pop();

	// put bytecode into heap
	for (int i = 0; i < interpreter.heap_ptrs[num + 1]; i++)
		interpreter.heap.erase(interpreter.heap.begin() + interpreter.heap_ptrs[num]);

	for (int i = 0; i < sval_size; i++) {
		interpreter.heap.insert(
			interpreter.heap.begin() + interpreter.heap_ptrs[num] + i,
			stop[i] + 0x80
		);
	}

	// adjust interpreter.heap_ptrs
	for (auto i = interpreter.heap_ptrs.begin() + num; i != interpreter.heap_ptrs.end(); i++)
		*i += sval_size - orig_size;

	interpreter.heap_ptrs[0] = 0;
}

void interpret(std::string contents) {
	for (auto loc = contents.begin(); loc != contents.end(); loc++) {
		switch (*loc) {
			case 0x0A:
				execute(loc);
				break;
			case 0x0C:
				allocateMemory(loc);
				break;
			case 0x0E:
				move(loc);
				break;
			case 0x1A:
				exit(0);
				break;
			case 0x18:
				popStack(loc);
				break;
		}
	}
}