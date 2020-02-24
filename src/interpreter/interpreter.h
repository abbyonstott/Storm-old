#pragma once
#include <stack>

inline struct interpreter_t {
	// virtual stack
	std::stack<std::string> stack;

	// virtual heap
	std::vector<uint8_t> heap;
	std::vector<int> heap_ptrs;

	// registers
	std::string registers[4] = {"\0", "\0", "\0", "\0"};
} interpreter;

void interpret(std::string contents);