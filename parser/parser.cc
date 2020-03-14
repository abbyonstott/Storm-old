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

void RunProgram(std::vector<std::string> splicedProgram) {
	for (auto chunk = splicedProgram.begin(); chunk != splicedProgram.end(); chunk++) {
		std::string kw = *chunk;

		if (kw == "write" || kw == "read") {
			StormCall(chunk);
		}
		else if (kw == "func") {
			// create_func(chunk);
		}
		else if ((chunk != splicedProgram.end()) && (*(chunk + 1) == "=")) {
			declare(chunk, *chunk);
			chunk++;
		}
	}
}

int variable::TotalNumber = 0;
int function::TotalNumber = 1;

void compile(std::vector<std::string> splicedProgram) {

	RunProgram(splicedProgram);
	parser.text.push_back(0x1A);
	
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
