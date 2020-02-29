/*
    storm.cc - main function
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

#include "storm.h"
#include "file/file.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"

int main(int argc, char const *argv[]) {
	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			if (argc < 4) {
				std::cerr << "Command should be formatted as:\nstorm -c <in_filename> <out_filename>";
				return EXIT_FAILURE;
			}

			program.filename = argv[2];
			parser.outfile = argv[3];
			splice(readFile());
		}
		else {
			program.filename = argv[1];
			interpret(readFile());
		}
	}
	else {
		std::cerr << "Expected arguments\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}