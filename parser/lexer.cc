/*
    splice.cc - create a vector of the program and parse special characters from strings
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
#include <regex>

char parseSpecial(std::string::iterator lttr) {
	char value = 0;

	lttr++; // go to next char
	switch (*lttr) {
		case 'n':
			value = '\n';
			break;
		case '\\':
			value = '\\';
			break;
	}
	lttr++;

	return value;
}

void lexer(std::string contents) {
	std::vector<std::string> splicedProgram;
	splicedProgram.resize(splicedProgram.size() + 1);
	bool inQuotes = 0;

	for (auto lttr = contents.begin(); lttr != contents.end(); lttr++) {
		// keyword or identifier
		if (*lttr == ';') {
			if (splicedProgram.back().size() != 0)
				splicedProgram.push_back(";");
			else
				splicedProgram.back() = ";";

			if (lttr + 1 != contents.end())
				splicedProgram.resize(splicedProgram.size() + 1);
		}
		else if (*lttr == '\n' || *lttr == '\t') {
			// remove last empty line
			if (lttr == contents.end()-1)
				splicedProgram.pop_back();
		}
		else if (*lttr == '\"') {
			inQuotes = ((inQuotes) ? 0 : 1);
			splicedProgram.back() += *lttr;
		}
		else if (*lttr == ' ') {
			if (inQuotes == true)
				splicedProgram.back() += *lttr;
			else if (splicedProgram.back() == "func")
				splicedProgram.resize(splicedProgram.size() + 1);
		}
		else if (*lttr == '\\') {
			if (inQuotes == true)
				splicedProgram.back() += parseSpecial(lttr);
			lttr++;
		}
		else if (*lttr == ',' || *lttr == '{' || *lttr == '}' || *lttr == '[' || *lttr == ']' || *lttr == '=') {
			if (inQuotes != true)
				splicedProgram.resize(splicedProgram.size() + 1);
			splicedProgram.back() += *lttr;
			if (inQuotes != true && *(lttr + 1) != ']' && *(lttr) != ']')
				splicedProgram.resize(splicedProgram.size() + 1);
		}
		else
			splicedProgram.back() += *lttr;
	}

	parser.splicedProgram = splicedProgram;
}