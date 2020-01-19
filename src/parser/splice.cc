#include "../stormscript.h"
#include "parser.h"

void splice(std::string contents) {
	std::vector<std::string> splicedProgram;
	splicedProgram.resize(splicedProgram.size() + 1);
	bool inQuotes = 0;

	for (char lttr : contents) {
		switch (lttr) {
			// keyword or identifier
			case 'a' ... 'z':
			case 'A' ... 'Z':
			case '0' ... '9':
				splicedProgram.back() += lttr;
				break;
			case ';':
				splicedProgram.resize(splicedProgram.size() + 1);
				break;
			// escape seq
			case  0 ... 31:
			case '\"':
				inQuotes = ((inQuotes) ? 0 : 1);
				splicedProgram.back() += lttr;
				break;
			case ' ':
				if (inQuotes == true)
					splicedProgram.back() += lttr;
				break;
			case ',':
			case '[':
			case ']':
				splicedProgram.resize(splicedProgram.size() + 1);				
				splicedProgram.back() += lttr;
				splicedProgram.resize(splicedProgram.size() + 1);
				break;
			default:
				splicedProgram.back() += lttr;
				break;
		}
	}
	compile(splicedProgram);
}