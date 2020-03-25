#include "../Storm/storm.h"
#include "parser.h"
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
			else if (splicedProgram.back() == "func")
				splicedProgram.resize(splicedProgram.size() + 1);
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