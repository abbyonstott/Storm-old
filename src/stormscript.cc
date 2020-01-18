#include "stormscript.h"
#include "file/file.h"

int main(int argc, char const *argv[]) {
	if (argc >= 2)
		program.filename = argv[1];
	else {
		std::cerr << "Expected file name\n";
		return -1;
	}

	readFile();

	return 0;
}