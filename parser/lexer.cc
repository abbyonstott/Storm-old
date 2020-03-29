#include "../Storm/storm.h"
#include "parser.h"
#include "../file/file.h"
#include <regex>

char parseSpecial(std::string::iterator lttr) {
	char value = 0;

	lttr++; // go to next char
	switch (*lttr) {
		case 'n':
			value = '\n';
			break;
		case '\\':
			value = '\\';
			break;
	}
	lttr++;

	return value;
}

void lexer(std::string contents) {
	std::vector<std::string> splicedProgram;
	splicedProgram.resize(splicedProgram.size() + 1);
	bool inQuotes = 0;

	for (auto lttr = contents.begin(); lttr != contents.end(); lttr++) {
		// keyword or identifier
		if (*lttr == ';') {
			if (splicedProgram.back().size() != 0)
				splicedProgram.push_back(";");
			else
				splicedProgram.back() = ";";

			if (lttr + 1 != contents.end())
				splicedProgram.resize(splicedProgram.size() + 1);
		}
		else if (splicedProgram.back() == "import") {
			// imports are evaluated here and inserted at this *lttr in the program
			splicedProgram.pop_back(); // remove import keyword
			std::string importName;

			while (*lttr != ';') {
				if (isalnum(*lttr) || *lttr == '_')
					importName += *lttr;
				lttr++;
			}
			if (importName.size() == 0) {
				std::cerr << "Error: import used but no file specified.\n";
				exit(1);
			}

			std::string fileContents = readFile(program.filepath + importName + ".storm");
			
			// a bit of a hack, but it works
			lexer(fileContents);
			splicedProgram.insert(splicedProgram.end(), 
				parser.splicedProgram.begin(), 
				parser.splicedProgram.end());

			parser.splicedProgram = {}; // reset after adding
			splicedProgram.resize(splicedProgram.size() + 1);
		}
		else if (*lttr == '\n' || *lttr == '\t') {
			// remove last empty line
			if (lttr == contents.end()-1)
				splicedProgram.pop_back();
		}
		else if (*lttr == '\"') {
			// things that are in quotes are not actually parsed
			inQuotes = ((inQuotes) ? 0 : 1);
			splicedProgram.back() += *lttr;
		}
		else if (*lttr == ' ') {
			if (inQuotes == true)
				splicedProgram.back() += *lttr;
			// spaces should add a new "chunk" for these symbols
			else if (splicedProgram.back() == "func"
				|| splicedProgram.back() == "return")
			{
				splicedProgram.resize(splicedProgram.size() + 1);
			}
		}
		else if (*lttr == '\\') {
			// SPecial characters
			if (inQuotes == true)
				splicedProgram.back() += parseSpecial(lttr);
			lttr++;
		}
		else if (*lttr == ',' || *lttr == '{' || *lttr == '}' || *lttr == '(' || *lttr == ')' || *lttr == '=') {
			// these are operators and symbols that need their own location in splicedProgram
			if (inQuotes != true && splicedProgram.back() != "")
				splicedProgram.resize(splicedProgram.size() + 1);
			splicedProgram.back() += *lttr;
			if (inQuotes != true && *(lttr + 1) != ')' && *(lttr) != ')')
				splicedProgram.resize(splicedProgram.size() + 1);
		}
		else if (*lttr == '/' && *(lttr + 1) == '/') {
			// comment skips the line
			do {
				lttr++;
			} while (lttr + 1 != contents.end() && *lttr != '\n');
		}
		else
			splicedProgram.back() += *lttr;
	}

	// this is only true for files with comments as their last lines
	if (splicedProgram.back() == "") splicedProgram.pop_back();

	parser.splicedProgram = splicedProgram;
}