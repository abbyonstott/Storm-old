#pragma once
#include "svalues.h"
#include "functions.h"
#include "storm_calls.h"

inline struct parser_t {
	std::string outfile;
	std::vector<std::string> splicedProgram;

	/*
	 * ".data" in nasm, initialized with data indicator for full program
	 *
	 * structured like this in bytecode
	 *
	 * data
	 * 	[0] type "value" ; for initialized
	 *	[1] res ; for uninitialized
	 *
	 * text
	 *	...
	*/
	std::vector<uint8_t> data = {0x0C};

	// ".text" in nasm, initialized with text indicator for full program and main function declaration
	std::vector<uint8_t> text;

	// first function should be {1}, as main is {0}
	std::vector<function> functions;

	std::vector<variable> vars;
} parser;

template<class T>
T find(std::string name) { return T(); };
template<>
variable find<variable>(std::string name);
template<>
function find<function>(std::string name);

// Separate the program into lines;
void lexer(std::string contents);