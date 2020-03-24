/*
	storm_calls.cc - parser for system call like functions
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

StormVMCall determineCall(std::string kw) {
	if (kw == "read")
		return STORM_READ;
	else
		return STORM_WRITE;
}

void addCallArgsToData(StormVMCall call, std::vector<std::string>::iterator &chunk) {
	size_t numargs = call.typesWanted.size();

	// Determine if call gives correct number of args
	if (numArgsGiven(chunk) != numargs) {
		// trigger error if not correct number of args
		std::cerr << "Compilation error: " << call.name << " requires " << numargs << " arguments\n";
		exit(EXIT_FAILURE);
	}
	chunk++;
	for (int i = 0; i < numargs; i++, chunk++) {
		variable arg;
	
		// If literal add it to data as a var and then add to varIdent
		if ((*chunk)[0] == '"' || isInt(*chunk)) {
			declare(chunk, "");
			arg = parser.vars.back();
		}
		else if ((*(chunk + 1))[0] == '(') { // function
			inlineFunc(chunk, arg);

			continue;
		}
		else { // variable
			try {
				arg = find<variable>(*chunk);
			}
			catch (NameError& e) {
				std::cerr << e.what() << "variable " << *chunk << " not found!\n";
				exit(EXIT_FAILURE);
			}
		}

		// throw incorrect type error if not reserved (as to preserve arguments in function declarations)
		if (arg.type != call.typesWanted[i] && arg.type != StormType::RESERVE && arg.type != StormType::SVOID) {
			std::cerr << "Error: " << call.name << " expects " << (int)call.typesWanted[i] << " for argument " << i+1 << ". Got " << (int)arg.type << " instead.\n";
			exit(EXIT_FAILURE);
		}

		parser.text.push_back(0x0E);
		parser.text.push_back(0x10 + i);
		parser.text.insert(parser.text.end(), arg.ident.begin(), arg.ident.end());

		// get through comma
		chunk++;
	}
	parser.text.push_back(0x0A); // execute
}

void StormCall(std::vector<std::string>::iterator &chunk) {
	StormVMCall call = determineCall(*chunk);
	parser.text.push_back(0x0E); // move
	parser.text.push_back(0x0F); // reg0

	parser.text.push_back(call.byte); // byte value of call

	chunk++;
	addCallArgsToData(call, chunk);
	while (*chunk != ";") chunk++;
}