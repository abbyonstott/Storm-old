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
#define STORM_MAIN_SOURCE // don't add unnecessary includes

#include "storm.h"
// strerror()
#include "string.h"

int main(int argc, char const *argv[]) {
	// status of execlp
	int run;
	std::string program;

	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			if (argc != 4) {
				std::cerr << "Command should be formatted as:\nstorm -c <file_name> <binary_name>\n";
				return EXIT_FAILURE;
			}

			#ifdef ISDEBUG
			program = "./stormcompiler";
			#else
			program = "stormcompiler";
			#endif

			run = execlp(program.c_str(), "stormcompiler", argv[2], argv[3], NULL);

		}
		else if (argc == 2) {
			#ifdef ISDEBUG
			program = "./stormrun";
			#else
			program = "stormrun";
			#endif

			run = execlp(program.c_str(), "stormrun", argv[1], NULL);
		}
	}
	else {
		std::cerr << "Format:\nstorm [options]...\n";
		return EXIT_FAILURE;
	}

	if (run == -1) {
		std::cerr << program << ": " << strerror(errno) << '\n';

		if (errno == 2) {
			// no such file or directory on program
			#ifdef ISDEBUG
			std::cerr << "Debug is enabled!\nMake sure have built all targets and are running from the build/ directory.\n";
			#else
			std::cerr << "Error: install is broken. Core file " << program << " not found!\n";
			#endif
		}

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}