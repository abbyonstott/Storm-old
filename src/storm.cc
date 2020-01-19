#include "storm.h"
#include "file/file.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"

int main(int argc, char const *argv[]) {
	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			if (argc != 3) {
				std::cerr << "Option '-c' requires filename\n";
				return EXIT_FAILURE;
			}

			program.filename = argv[2];
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