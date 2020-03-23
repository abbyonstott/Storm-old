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

#include "../Storm/storm.h"
#include "parser.h"

#include "../file/file.h"

void function::parse(std::vector<std::string>::iterator &chunk) {
	std::vector<variable> parentScope = parser.vars;

	for (chunk; chunk != parser.splicedProgram.end(); chunk++) {
		if (*chunk == "write" || *chunk == "read") {
			StormCall(chunk);
		}
		else if (*chunk == "func") {
			chunk++; // skip from "func" keyword to name
	
			parser.functions.push_back(function(*chunk));

			create_func(chunk,  &parser.functions.back());
		}
		else if ((chunk != parser.splicedProgram.end()) && (*(chunk + 1) == "=")) {
			declare(chunk, *chunk);
		}
		else if (*chunk == "}") {
			// exit scope
			parser.text.push_back(0x19); // ret
			break;
		}
		else if (*(chunk + 1) == "(") { // run function
			function *f;

			try {
				f = new function(find<function>(*chunk));
			}
			catch(NameError& e) {
				std::cerr << e.what() << "Function " << *chunk << " not found!\n";
				exit(EXIT_FAILURE);
			}

			f->run(chunk);

			while (*chunk != ";") { chunk++; }
		}
		else {
			std::cerr << NameError().what() << *chunk << " is not a recognized keyword or defined value.\n";
			exit(EXIT_FAILURE);
		}
	}
}

void compile() {
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

	stormc.close();
}

int variable::TotalNumber = 0;
int function::TotalNumber = 0;

int main(int argc, char const *argv[]) {
	if (argc != 3) {
		std::cerr << "Format:\nstormcompiler <filename> <binaryname>\n";
		return EXIT_FAILURE;
	}

	program.filename = std::string(argv[1]);
	parser.outfile = std::string(argv[2]);
	lexer(readFile());

	function StormMain("main");

	for (uint8_t byte : StormMain.ident)
		parser.text.push_back(byte);

	StormMain.offset = 0;
	parser.functions.push_back(StormMain);

	std::vector<std::string>::iterator chunk = parser.splicedProgram.begin();

	parser.functions.back().parse(chunk);
	parser.text.push_back(0x1A); // exit
	parser.text.insert(parser.text.begin(), 0x0B); // text

	compile();

	std::cout << "Program " << parser.outfile << " compiled successfully.\n";
	return EXIT_SUCCESS;
}
// bytecode is similar to nasm
