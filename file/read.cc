/*
    read.cc - open program for reading
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
#include "file.h"

std::string readFile() {
	std::ifstream file;
	std::string contents;

	file.open(program.filename);

	if (file.fail()) {
		std::cerr << "Failed to open file\n";
		exit(EXIT_FAILURE);
	}
	
	char c = file.get();

	while (file.good()) {
		contents += c;
		c = file.get();
	}

	file.close();

	return contents;
}