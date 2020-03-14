/*
    svalues.h - Struct for svalues.cc
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

class variable {
public:
	std::vector<uint8_t> ident;
	std::string name;

	Type type;

	// total number declared
	static int TotalNumber;

	variable(std::string _name);
};

void find(std::string name, variable &buf);

// declare variables
void declare(std::vector<std::string>::iterator &chunk, std::string name);
// add literal to data
void addLitToData(std::string literal);