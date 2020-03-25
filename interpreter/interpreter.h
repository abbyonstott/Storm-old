#pragma once
#include <stack>

inline struct interpreter_t {
	// virtual stack
	std::stack<std::string> stack;
        std::string contents;

	// virtual heap
	std::vector<uint8_t> heap;
	std::vector<int> heap_ptrs;

        int exit_code = -1;

	// registers
	std::string registers[5] = {"\0", "\0", "\0", "\0", "\0"};

        std::vector<std::string::iterator> functions;
} interpreter;

std::string getVal(std::string::iterator &loc);
void runScope(std::string::iterator &startloc);
int getLoc(std::string::iterator &loc, char endtoken);
void redefVar(int num, std::vector<uint8_t> data);

void allocateMemory(std::string::iterator &loc);
void createFunctions(std::string::iterator &loc);
void popStack(std::string::iterator &loc);
void callFunc(std::string::iterator &loc);