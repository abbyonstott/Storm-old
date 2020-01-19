#include "storm.h"
#include "file/file.h"
#include "parser/parser.h"

int main(int argc, char const *argv[]) {
	if (argc >= 2)
		program.filename = argv[1];
	else {
		std::cerr << "Expected file name\n";
		return EXIT_FAILURE;
	}

	splice(readFile());

	return EXIT_SUCCESS;
}