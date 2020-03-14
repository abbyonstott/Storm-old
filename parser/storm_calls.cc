#include "../Storm/storm.h"
#include "parser.h"

StormVMCall determineCall(std::string kw) {
	if (kw == "read")
		return STORM_READ;
	else
		return STORM_WRITE;
}

void addCallArgsToData(StormVMCall call, std::vector<std::string>::iterator &chunk) {
	int numargs = call.typesWanted.size();

	// Determine if call gives correct number of args
	if (numArgsGiven(chunk) != numargs) {
		// trigger error if not correct number of args
		std::cerr << "Compilation error: " << call.name << " requires " << numargs << " arguments\n";
		exit(EXIT_FAILURE);
	}
	chunk++;
	for (int i = 0; i < numargs; i++, chunk++) {
		variable *arg;
	
		// If literal add it to data as a var and then add to varIdent
		if ((*chunk)[0] == '"' || isInt(*chunk)) {
			declare(chunk, "");
			arg = &parser.vars.back();
		}
		else if ((*(chunk + 1))[0] == '[') { // function
			inlineFunc(chunk);
			continue;
		}
		else // variable
			find(*chunk, *arg);

		// throw incorrect type error
		if (arg->type != call.typesWanted[i]) {
			std::cerr << "Error: " << call.name << " expects " << call.typesWanted[i] << " for argument " << i+1 << ". Got " << arg->type << " instead.\n";
			exit(EXIT_FAILURE);
		}

		parser.text.push_back(0x0E);
		parser.text.push_back(0x10 + i);
		parser.text.insert(parser.text.end(), arg->ident.begin(), arg->ident.end());

		// get through comma
		chunk++;
	}
	parser.text.push_back(0x0A); // execute
}

void StormCall(std::vector<std::string>::iterator &chunk) {
	StormVMCall call = determineCall(*chunk);
	parser.text.push_back(0x0E); // move
	parser.text.push_back(0x0F); // reg0

	parser.text.push_back(call.byte); // byte value of call

	chunk++;
	addCallArgsToData(call, chunk);
	while (*chunk != ";") chunk++;
}