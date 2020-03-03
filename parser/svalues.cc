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
#include "svalues.h"
#include "parser.h"

// format var as [num] and return bytecode representation
std::vector<uint8_t> formatVar(int num) {
	std::vector<uint8_t> ident;

	char hval_ident[8];
	// get hex of var
	sprintf(hval_ident, "%X", num); 

	ident.push_back('[' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back(']' + 0x80);

	return ident;
}

// change value identifier
void changeByteVal() {
	std::vector<uint8_t> new_val_ident = formatVar(svalues.val_ident);
	svalues.byte_val_ident = new_val_ident;
}

// Convert std::string to bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}

// find var by name and return ident
std::vector<uint8_t> findVar(std::string name) {
	std::vector<uint8_t> ident;

	std::vector<std::string>::iterator vloc = std::find(
		svalues.names.begin(), 
		svalues.names.end(), 
		name
	); // find var
	
	// not found
	if (vloc == svalues.names.end()) {
		std::cerr << "Variable " << name << " not found.\n";
		exit(EXIT_FAILURE); 
	}

	return formatVar(vloc - svalues.names.begin());
}

void addLitToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);

	changeByteVal();
	svalues.val_ident++;

	parser.data.insert(parser.data.end(), 
		svalues.byte_val_ident.begin(), svalues.byte_val_ident.end());

	// type byte
	parser.data.push_back(0x0D);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}

void addArgsToData(std::vector<std::string>::iterator &chunk, int numargs, std::vector<Type> typesWanted) {
	chunk++;
	for (int i = 0; i < numargs; i++, chunk++) {
		// Identifier of var
		std::vector<uint8_t> varIdent;
	
		// If literal add it to data as a var and then add to varIdent
		if ((*chunk)[0] == '"') {
			if (typesWanted[i] != STRING) { 
				//TODO: add name of function to error
				std::cerr << "Error: Expected int for argument 1.\n";
				exit(EXIT_FAILURE);
			}
			
			std::string literal = *chunk;
			addLitToData(literal);
			svalues.names.push_back("");
			varIdent = svalues.byte_val_ident;
		}
		else if (typesWanted[i] == INTEGER && isInt(*chunk)) {
			changeByteVal();
			svalues.val_ident++;
			
			parser.data.insert(parser.data.end(),
				svalues.byte_val_ident.begin(), svalues.byte_val_ident.end());
			parser.data.push_back(0x13);

			// push_back value of int
			for (char c : *chunk)
				parser.data.push_back(c - '0');

			svalues.names.push_back("");
			varIdent = svalues.byte_val_ident;
		}
		else
			varIdent = findVar(*chunk);

		parser.text.push_back(0x0E);
		parser.text.push_back(0x10 + i);
		parser.text.insert(parser.text.end(), varIdent.begin(), varIdent.end());

		// get through comma
		chunk++;
	}
	parser.text.push_back(0x0A); // execute
}

void declare(std::vector<std::string>::iterator &chunk) {
	std::string name = *chunk;
	std::vector<uint8_t> valBytecode;
	chunk += 2;

	svalues.names.push_back(name); // add name to list of names

	if ((*chunk)[0] == '\"') // string literal
		addLitToData(*chunk);
	else {
		std::vector<uint8_t> value = findVar(*chunk);
		changeByteVal();
		svalues.val_ident++;
		
		parser.data.insert(parser.data.end(), 
			svalues.byte_val_ident.begin(), svalues.byte_val_ident.end());
		parser.data.push_back(0x0D);
		parser.data.insert(parser.data.end(),
			value.begin(), value.end());
	}
}