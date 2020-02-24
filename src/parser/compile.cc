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
		// commas inside quotes don't count, wait until quotes end for next splice
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

std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}

void getData(std::vector<std::string> splicedProgram) {
	for (auto chunk = splicedProgram.begin(); chunk != splicedProgram.end(); chunk++) {
		if (*chunk == "write") { // requires 2 args
			parser.text.push_back(0x0E); // move
			parser.text.push_back(0x0F); // veax
			parser.text.push_back(0x41);

			chunk++;
			if (*chunk != "[" || numArgsGiven(chunk) != 2) {
				// trigger error if not correct number of args
				std::cerr << "Compilation error: write requires 2 arguments\n";
				exit(EXIT_FAILURE);
			}

			// add arg lit to data
			for (int i = 0; i < 2; i++, chunk++) {
				isCorrectType(STRING, chunk);
				
				std::string literal = evalLit(chunk);
				addArgToData(literal);

				parser.text.push_back(0x0E);
				parser.text.push_back( ((i == 0) ? 0x10 : 0x11) );
				parser.text.insert(parser.text.end(), 
					parser.byte_val_ident.begin(), parser.byte_val_ident.end());
			}
			parser.text.push_back(0x0A); // execute
		}
	}
}

void compile(std::vector<std::string> splicedProgram) {
	getData(splicedProgram);
	
	// push compiled code to file
	std::string compiledFileName = program.filename + "c";
	std::vector<uint8_t> fullprogram;
	
	// start formatting data section
	fullprogram.insert(fullprogram.end(),
		parser.data.begin(), parser.data.end());

	// format text
	fullprogram.insert(fullprogram.end(),
		parser.text.begin(), parser.text.end());

	std::ofstream stormc;
	stormc.open(compiledFileName);

	for (uint8_t byte : fullprogram) {
		stormc << byte;
	}
}
// bytecode is similar to nasm