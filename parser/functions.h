/*
    functions.h - Function class and functions
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

class function {
public:
	std::vector<uint8_t> ident;
	std::string name;

	// return type
	Type type;
	// args
	std::vector<Type> args;

	// total number declared
	static int TotalNumber;
	
	std::vector<uint8_t>::iterator loc;

	function(std::string _name);
};

// evaluate number of args given from function
int numArgsGiven(std::vector<std::string>::iterator chunk);
// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk, variable &v);