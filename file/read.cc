#include "../Storm/storm.h"
#include "file.h"

std::string readFile() {
	std::ifstream file;
	std::string contents;

	file.open(program.filename);

	if (file.fail()) {
		std::cerr << "Error: Failed to open file " << program.filename << '\n';
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