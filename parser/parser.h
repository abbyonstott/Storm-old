/*
    parser.h - The parser struct and parser functions
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
#pragma once
#include "svalues.h"
#include "functions.h"
#include "storm_calls.h"

inline struct parser_t {
	std::string outfile;

	/*
	  ".data" in nasm, initialized with data indicator for full program
	 
	  structured like this in bytecode
	 
	  data
	  	[0] type "value" ; for initialized
	  	[1] res ; for uninitialized
	 
	  text
	  	...
	*/
	std::vector<uint8_t> data = {0x0C};

	// ".text" in nasm, initialized with text indicator for full program and main function declaration
	std::vector<uint8_t> text = {0x0B, '{' + 0x80, '0' + 0x80, '}' + 0x80};

	// first function should be {1}, as main is {0}
	std::vector<function> functions;

	std::vector<variable> vars;
} parser;

// general parser/compiler functions

// Separate the program into lines;
void splice(std::string contents);
// Compile the program into a file of bytecode
void compile(std::vector<std::string> splicedProgram);