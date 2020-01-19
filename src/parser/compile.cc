#include "../storm.h"
#include "parser.h"

void isCorrectType(int type, std::vector<std::string>::iterator chunk) {
	chunk++;

	switch (type)
	{
		case STRING:
			if ((*chunk)[0] != '\"') {
				std::cerr << "Compilation error: expected string as first argument\n";
				exit(EXIT_FAILURE);
			}
			
			while (chunk->back() != '\"')
				chunk++;
			
			break;
	}
}

int numArgsGiven(std::vector<std::string>::iterator chunk) {
	int arg = 1;
	bool inquotes = 0;

	do {
		if ((*chunk)[0] == '\"' ^ chunk->back() == '\"') {
			inquotes = !inquotes;
			chunk++;
			continue;
		}
		
		if (*(chunk) == "," && inquotes == 0)
			arg++;

		chunk++;
	} while (*chunk != "]");

	return arg;
}

std::string evalLit(std::vector<std::string>::iterator &chunk) {
	// get string without quotes
	std::string literal;
	chunk++;

	while(chunk->back() != '\"') {
		literal += *(chunk);
		chunk++;
	}

	literal += *(chunk);

	return literal;
}

void addStringToByteCode(std::string lit) {
	for (char c : lit)
		program.bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80
}

void getByteCode(std::vector<std::string> splicedProgram) {
	for (auto chunk = splicedProgram.begin(); chunk != splicedProgram.end(); chunk++) {
		if (*chunk == "write") {
			// evaluate based on number of args required. In this case: 2
			program.bytecode.push_back(0x0A);

			chunk++;
			if (*(chunk) != "[" || numArgsGiven(chunk) != 2) {
				// trigger error if no args given
				std::cerr << "Compilation error: write requires 2 arguments\n";
				exit(EXIT_FAILURE);
			}

			for (int i = 0; i < 2; i++, chunk++) {
				isCorrectType(STRING, chunk);

				std::string literal = evalLit(chunk);

				addStringToByteCode(literal);
			}

			addStringToByteCode(";");
		}
		else { // identifier

		}
	}
}

void compile(std::vector<std::string> splicedProgram) {
	std::string compiledFileName = program.filename + "c";
	getByteCode(splicedProgram);

	std::ofstream stormc;
	stormc.open(compiledFileName);

	for (uint8_t byte : program.bytecode)
		stormc << byte;

	stormc.close();
}