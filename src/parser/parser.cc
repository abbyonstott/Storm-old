/*
    parser.cc - Parse and Compile the program
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

#include "../storm.h"
#include "parser.h"

int numArgsGiven(std::vector<std::string>::iterator chunk) {
	int arg = 1;
	bool inquotes = 0;

	do {	
		if (*(chunk) == "," && inquotes == 0)
			arg++;

		chunk++;
	} while (*chunk != "]");

	return arg;
}

// check if number of args is accurate
void checkargs(std::vector<std::string>::iterator chunk, int required, std::string kw) {
	if (*chunk != "[" || numArgsGiven(chunk) != 2) {
		// trigger error if not correct number of args
		std::cerr << "Compilation error: write requires 2 arguments\n";
		exit(EXIT_FAILURE);
	}
}

// change value identifier
void changeByteVal() {
	std::vector<uint8_t> new_val_ident;
	char hval_ident[8];

	new_val_ident.push_back('[' + 0x80);

	// hex string representation of parser.val_ident
	sprintf(hval_ident, "%X", parser.val_ident); 
	std::string hval_ident_s = hval_ident;

	for (char n : hval_ident_s)
		new_val_ident.push_back(n + 0x80);

	new_val_ident.push_back(']' + 0x80);

	parser.byte_val_ident = new_val_ident;
}

std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}



void isCorrectType(int type, std::vector<std::string>::iterator &chunk) {
	chunk++;

	switch (type)
	{
		case STRING:
			if ((*chunk)[0] != '\"') {
				std::cerr << "Compilation error: expected string as argument\n";
				exit(EXIT_FAILURE);
			}
			
			break;
	}
}

void addLitToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);

	changeByteVal();
	parser.val_ident++;

	parser.data.insert(parser.data.end(), 
		parser.byte_val_ident.begin(), parser.byte_val_ident.end());

	// type byte
	parser.data.push_back(0x0D);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}

// add arg literal to data
void addArgsToData(std::vector<std::string>::iterator &chunk, int numargs) {
	for (int i = 0; i < numargs; i++, chunk++) {
		isCorrectType(STRING, chunk);
		
		std::string literal = *chunk;
		addLitToData(literal);

		parser.text.push_back(0x0E);
		parser.text.push_back(0x10 + i);
		parser.text.insert(parser.text.end(), 
			parser.byte_val_ident.begin(), parser.byte_val_ident.end());
	}
	parser.text.push_back(0x0A); // execute
}

void declare(std::vector<std::string>::iterator &chunk) {
	std::string val, name = *chunk;
	std::vector<uint8_t> valBytecode;
	chunk += 2;

	parser.names.push_back(*chunk); // add name to list of names

	if ((*chunk)[0] == '\"') { // string literal
		val = *chunk;
		addLitToData(val);
	}
	else {
		std::cerr << "Error: non-literal initialization of variables is not supported.\n";
		exit(1);
	}
}

void getData(std::vector<std::string> splicedProgram) {
	for (auto chunk = splicedProgram.begin(); chunk != splicedProgram.end(); chunk++) {
		std::string kw = *chunk;
		if (kw == "write") {
			parser.text.push_back(0x0E); // move
			parser.text.push_back(0x0F); // veax

			parser.text.push_back(0x41);

			chunk++;
			checkargs(chunk, 2, kw);
			addArgsToData(chunk, 2);
			do { chunk++; } while (*chunk != ";");
		}
		else if ((chunk != splicedProgram.end()) && (*(chunk + 1) == "=")) {
			declare(chunk);
			chunk++;
		}
	}
}

void compile(std::vector<std::string> splicedProgram) {
	getData(splicedProgram);
	
	// push compiled code to file
	std::string compiledFileName = parser.outfile;
	std::vector<uint8_t> fullprogram;
	
	// start formatting data section
	fullprogram.insert(fullprogram.end(),
		parser.data.begin(), parser.data.end());

	// format text
	fullprogram.insert(fullprogram.end(),
		parser.text.begin(), parser.text.end());

	std::ofstream stormc;
	stormc.open(compiledFileName);

	for (uint8_t byte : fullprogram) {
		stormc << byte;
	}
}
// bytecode is similar to nasm