#include "../Storm/storm.h"
#include "interpreter.h"
#include "../file/file.h"
#include "arithmetic.h"

#ifdef _WIN32
HANDLE hStdout, hStdin;
#endif // _WIN32

// mov value to register
void move(std::string::iterator loc) {
	loc++;

	// Moving value to register?
	if (*loc >= 0x0F && *loc <= 0x13 ) {
		// determine register
		int reg = *loc - 0x0F;

		loc++;
		
		// load value from next bytecode
		switch (*loc) {
			case 0x40: // read
			case 0x41: // write
			case 0x42: // assert
				interpreter.registers[reg] = *loc;
				loc++;
				break;
			case char('[' + 0x80):
				interpreter.registers[reg] = getVal(loc);

				/* 
				 * only do this when in register, could be a security issue 
				 * later on if unstripped strings with "[" chars are left in 
				 * the heap. This will probably be removed entirely due to
				 * this causing type errors when unmanaged
				*/
				if (interpreter.registers[reg][0] == '\"') 
					stripString(&interpreter.registers[reg]);
				
				break;
		}
	}
	// redefining
	else if (*loc == char('[' + 0x80)) {
		// move [lh] [rh]
		int lh = getLoc(loc, ']');

		*loc++;

		std::vector<uint8_t> data;

		if (*loc == char('[' + 0x80)) {
			int rh = getLoc(loc, ']');
			int heapIter = interpreter.heap_ptrs[rh];

			while (heapIter != interpreter.heap_ptrs[rh + 1])
				data.push_back(interpreter.heap[heapIter++]);
		}
		// string literal
		else if (*loc == char('"' + 0x80)) {
			// preserve ""
			do {
				data.push_back(*loc);
				loc++;
			} while (char(*(loc) - 0x80) != '\"');
			data.push_back(*loc);
		}
		else if (*loc <= 0x09) {
			while (*loc <= 0x09)
				data.push_back(*(loc++));
		}

		redefVar(lh, data);
	}
}

/*
 * Run a call.
 * This relies on the size of the first register being one byte.
*/
void execute(std::string::iterator loc) {
	// change command executed based on value of register 0 (always 1 byte)

	// make sure execute is called legally
	assert(interpreter.registers[0].size() == 1);

	switch (interpreter.registers[0][0]) {
		case 0x40: // read
		{
			char* buf;
#ifndef _WIN32
			int fd = open(interpreter.registers[1].c_str(), O_RDONLY);
			buf = new char[std::stoi(interpreter.registers[2])];

			read(fd, (void*)buf, std::stoi(interpreter.registers[2]));
			close(fd);
#else

			// if on windows, redirect /dev/stdout to stdout
			std::string filename = interpreter.registers[1];
			HANDLE fileHandle;

			// Visual C++ doesn't like non-constexpr subscripts for arrays, but accepts them for pointers.
			char* chBuffer = new char[std::stoi(interpreter.registers[2])];

			DWORD totalRead;

			if (filename == "/dev/null") {
				filename = "nul";
			}

			if (filename == "/dev/stdin")
				fileHandle = hStdin;
			else {
				fileHandle = CreateFileA(
					filename.c_str(),
					GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);
			}

			if (!ReadFile(fileHandle, chBuffer, std::stoi(interpreter.registers[2]), &totalRead, NULL)) {
				std::cerr << "Error: Could not read from file " << filename << '\n';
				exit(EXIT_FAILURE);
			};
			
			CloseHandle(fileHandle);
			delete[] chBuffer;
			// save memory by allocating amount read rather than amount requested.
			buf = new char[totalRead];
#endif // _WIN32

			interpreter.stack.push(buf);

			// c strings append octal escapes that mess up the values, this will remove them
			while (interpreter.stack.top().size() > std::stoi(interpreter.registers[2])) 
				interpreter.stack.top().pop_back();

			interpreter.stack.top().insert(interpreter.stack.top().begin(), '"');
			interpreter.stack.top().push_back('"');

			delete[] buf;
			break;
		}
		case 0x41: // write
		{	
#ifndef _WIN32
			int fd = open(interpreter.registers[2].c_str(), O_WRONLY);

			write(fd,
				interpreter.registers[1].c_str(),
				interpreter.registers[1].size());
			
			close(fd);
#else
			std::string filename = interpreter.registers[2];
			HANDLE fileHandle;
			DWORD totalWritten;

			if (filename == "/dev/null") {
				filename = "nul";
			}

			if (filename == "/dev/stdout") {
				fileHandle = hStdout;
			}
			else {
				fileHandle = CreateFileA(
					filename.c_str(),
					GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
				);
			}

			if (!WriteFile(fileHandle, interpreter.registers[1].c_str(), (DWORD)interpreter.registers[1].size(), &totalWritten, NULL)) {
				std::cerr << "Error: Could not write to file " << filename << '\n';
				exit(EXIT_FAILURE);
			}

			CloseHandle(fileHandle);
#endif // _WIN32
			
			break;
		}
		case 0x42: // assert (no win32/posix sections :D)
		{
			if (interpreter.registers[3] == "1") // assert(value, value, true);
				assert(interpreter.registers[1] == interpreter.registers[2]);
			else
				assert(interpreter.registers[1] != interpreter.registers[2]);

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
			
			case (int)MathOper::ADD:
			case (int)MathOper::SUB:				
			case (int)MathOper::MULT:
			case (int)MathOper::DIV:
				arithmeticOperation(loc);	

			case 0x1A: // exit
				interpreter.exit_code = 0;
				return;
			case 0x19: // ret
				endScope = true;
				break;
			case 0x17: // push
				pushStack(loc);
				break;
			case 0x18: // pop
				popStack(loc);
				break;
			case 0x1C: // call
				callFunc(loc);
				// if exit() was called this value will change
				break;
		}

		if (interpreter.exit_code >= 0) return;
		if (endScope) break;
	}
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hStdin == INVALID_HANDLE_VALUE || hStdout == INVALID_HANDLE_VALUE) {
		std::cerr << "Unable to find STD file handles\n";
	}
#endif // _WIN32
	
	program.filename = argv[1];
	interpreter.contents = readFile();
	std::string::iterator loc = interpreter.contents.begin();

	allocateMemory(loc);
	createFunctions(loc);
	runScope(loc);

	return interpreter.exit_code;
}