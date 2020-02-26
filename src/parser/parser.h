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

inline struct parser_t {
	// next value identifier
	unsigned int val_ident = 0;

	// bytecode representation of the last value identifier used
	std::vector<uint8_t> byte_val_ident;

	// ".data" in nasm, initialized with data indicator for full program
	std::vector<uint8_t> data = {0x0C};

	// ".text" in nasm, initialized with data indicator for full program
	std::vector<uint8_t> text = {0x0B}; 
} parser;

// general parser/compiler functions

// Separate the program into lines;
void splice(std::string contents);
// Compile the program into a file of bytecode
void compile(std::vector<std::string> splicedProgram);
// Convert string to ASCII-based bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit);

// data functions

// Add argument to data section
void addArgToData(std::string literal);