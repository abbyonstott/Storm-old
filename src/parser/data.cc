/*
    data.cc - Operations on the data section of the program
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

void changeByteVal() {
	std::vector<uint8_t> new_val_ident;
	char hval_ident[8];

	new_val_ident.push_back('[' + 0x80);

	// hex string representation of parser.val_ident
	sprintf(hval_ident, "%X", parser.val_ident); 
	std::string hval_ident_s = hval_ident;

	for (char n : hval_ident_s) {
		new_val_ident.push_back(n + 0x80);
	}

	new_val_ident.push_back(']' + 0x80);

	parser.byte_val_ident = new_val_ident;
}

void addArgToData(std::string literal) {
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