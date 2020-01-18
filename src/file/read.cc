#include "../stormscript.h"
#include "file.h"

void readFile() {
	std::ifstream file;
	std::string contents;

	file.open(program.filename);

	if (file.fail()) {
		std::cerr << "Failed to open file\n";
		exit(-1);
	}
	
	char c = file.get();

	while (file.good()) {
		contents += c;
		c = file.get();
	}

	program.contents = contents;
}