/*
	svalues.cc - The compiler's memory management
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

/*
 * variable class constructor
 * set name
 * create ident
*/
variable::variable(std::string _name) {
	name = _name;

	char hval_ident[8];
	// get hex of var
#ifndef WIN32
	sprintf(hval_ident, "%X", TotalNumber);
#else
	sprintf_s(hval_ident, "%X", TotalNumber);
#endif // WIN32

	ident.push_back('[' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back(']' + 0x80);

	// add identifier to data
	parser.data.insert(parser.data.end(), 
		ident.begin(), ident.end());

	TotalNumber++;
}

// Convert std::string to bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}

// find var by name and return ident
template<>
variable find<variable>(std::string name) {
	for (variable v : parser.vars) {
		if (v.name == name && v.name != "")
			return v;
	}

	throw NameError();
}

void addLitToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}

// declare variable
void declare(std::vector<std::string>::iterator &chunk, std::string name) {
	variable v(name);

	if (name != "")
		chunk += 2;

	if ((*chunk)[0] == '\"') {// string literal
		v.type = StormType::STRING;
		parser.data.push_back((int)v.type);
		
		addLitToData(*chunk);
	}
	else if (isInt(*chunk)) {
		v.type = StormType::INTEGER;

		parser.data.push_back((int)v.type);
		
		// push_back value of int
		for (char c : *chunk)
			parser.data.push_back(c - '0');
	}
	else if (*(chunk+1) == "(") { // function
		inlineFunc(chunk, v);
		parser.vars.push_back(v);

		while (*chunk != ";") chunk++;
		
		return;
	}
	else {
		// search for already defined variable
		variable *match;

		try {
			match = new variable(find<variable>(*chunk));
		}
		catch (NameError& e) {
			std::cerr << e.what() << "variable " << *chunk << " not found!\n";
			exit(EXIT_FAILURE);
		}
		
		v.type = (match->type != StormType::RESERVE) 
			? match->type
			: StormType::SVOID;
		
		parser.data.push_back((int)v.type);
		parser.data.insert(parser.data.end(),
			match->ident.begin(), match->ident.end());
		
		delete match;
	}

	parser.vars.push_back(v);

	if (name != "")
		chunk++;
}