/*
    storm.c - main function
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
// don't add unnecessary includes from storm.h
#define STORM_MAIN_SOURCE

#include "storm.h"

int main(int argc, char const *argv[]) {
	// if argv is not just the executable, then it must be run from "./command" 
	const bool local = !(std::string(argv[0]) == "storm");

	// status of execlp
	int run;
	std::string program;
	std::vector<char*> _args;

	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			// compile
			if (argc != 4) {
				std::cerr << "Command should be formatted as:\nstorm -c <file_name> <binary_name>\n";
				return EXIT_FAILURE;
			}

			program = ((local) ? "./stormcompiler" : "stormcompiler");
			_args.push_back((char*)program.c_str());
			_args.push_back((char*)argv[2]);
			_args.push_back((char*)argv[3]);
		}
		else if (argc == 2) {
			// run code
			program = ((local) ? "./stormrun" : "stormrun");
			_args.push_back((char*)program.c_str());
			_args.push_back((char*)argv[1]);
		}
	}
	else {
		std::cerr << "Format:\nstorm [options]...\n";
		return EXIT_FAILURE;
	}

	_args.push_back(NULL);

	// convert args to char *const array
	char *args[_args.size()];
	std::copy(_args.begin(), _args.end(), args);

	if (execvp(program.c_str(), args) == -1) {
		std::cerr << program << ": " << strerror(errno) << '\n';

		// no such file or directory on stormcompiler or stormrun
		if (errno == 2)
			std::cerr << "Error: install is broken. Core file " << program << " not found!\n";

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}