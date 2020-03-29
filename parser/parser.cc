#include "../Storm/storm.h"
#include "parser.h"

#include "../file/file.h"

#include <iterator>

void function::parse(std::vector<std::string>::iterator &chunk) {
	std::vector<variable> parentScope = parser.vars;

	for (chunk; chunk != parser.splicedProgram.end(); chunk++) {
		if (*chunk == "write" || *chunk == "read") {
			StormCall(chunk);
		}
		else if (*chunk == "func") {
			chunk++; // skip from "func" keyword to name
	
			parser.functions.push_back(function(*chunk));

			create_func(chunk,  &parser.functions.back());
		}
		else if (*chunk == "return") {

			returnValue(chunk);
			parser.text.push_back(0x19);

			while (*chunk != ";") chunk++;
			chunk++;
			break;
		}
		else if ((chunk != parser.splicedProgram.end()) && (*(chunk + 1) == "=")) {
			declare(chunk, *chunk);
		}
		else if (*chunk == "}") {
			if (name == "main") {
				std::cerr << "Error: mismatched } in main scope";
				exit(EXIT_FAILURE);
			}
			// exit scope
			parser.text.push_back(0x19); // ret
			break;
		}
		else if (*(chunk + 1) == "(") { // run function
			function *f;

			try {
				f = new function(find<function>(*chunk));
			}
			catch(NameError& e) {
				std::cerr << e.what() << "Function " << *chunk << " not found!\n";
				exit(EXIT_FAILURE);
			}

			f->run(chunk);
			chunk++;
		}
		else {
			std::cerr << NameError().what() << *chunk << " is not a recognized keyword or defined value.\n";
			exit(EXIT_FAILURE);
		}
	}
}

void compile() {
	// push compiled code to file
	std::string compiledFileName = parser.outfile;
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

	stormc.close();
}

int variable::TotalNumber = 0;
int function::TotalNumber = 0;

int main(int argc, char const *argv[]) {
	if (argc != 3) {
		std::cerr << "Format:\nstormcompiler <filename> <binaryname>\n";
		return EXIT_FAILURE;
	}

	program.filename = std::string(argv[1]);
	int filenamesize = 0;

	// get size of filename without path
	for (std::string::reverse_iterator fileNameIter = program.filename.rbegin(); 
		*fileNameIter != '/'; 
		fileNameIter++, filenamesize++);

	program.filepath = program.filename.substr(0, program.filename.size() - filenamesize);


	parser.outfile = std::string(argv[2]);
	lexer(readFile(program.filename));

	function StormMain("main");

	for (uint8_t byte : StormMain.ident)
		parser.text.push_back(byte);

	StormMain.offset = 0;
	parser.functions.push_back(StormMain);

	std::vector<std::string>::iterator chunk = parser.splicedProgram.begin();

	parser.functions.back().parse(chunk);
	parser.text.push_back(0x1A); // exit
	parser.text.insert(parser.text.begin(), 0x0B); // text

	compile();

	std::cout << "Program " << parser.outfile << " compiled successfully.\n";
	return EXIT_SUCCESS;
}
// bytecode is similar to nasm
