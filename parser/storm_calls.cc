#include "../Storm/storm.h"
#include "parser.h"

StormVMCall determineCall(std::string kw) {
	if (kw == "read")
		return STORM_READ;
	else
		return STORM_WRITE;
}

void addCallArgsToData(StormVMCall call, std::vector<std::string>::iterator &chunk) {
	size_t numargs = call.typesWanted.size();
	std::vector<uint8_t> newText = {
		0x0E, // mov
		0x0F, // reg0
		call.byte 
	}; // what will be added to parser.text

	// Determine if call gives correct number of args
	if (numArgsGiven(chunk) != numargs) {
		// trigger error if not correct number of args
		std::cerr << "Compilation error: " << call.name << " requires " << numargs << " arguments\n";
		exit(EXIT_FAILURE);
	}

	chunk++;
	for (int i = 0; i < numargs; i++, chunk++) {
		variable arg;
	
		// If literal add it to data as a var and then add to varIdent
		if ((*chunk)[0] == '"' || isInt(*chunk)) {
			declare(chunk, "");
			arg = parser.vars.back();
		}
		else if ((*(chunk + 1)) == "(") { // function
			inlineFunc(chunk, arg);
			chunk++;
			// pop regn
			newText.push_back(0x18);
			newText.push_back(0x10 + i);
			/* 
			 * don't do the move thing that the others do because 
			 * no memory will be used here apart from variables 
			 * declared from inside the function
			*/
			continue;
		}
		else { // variable
			try {
				arg = find<variable>(*chunk);
			}
			catch (NameError& e) {
				std::cerr << e.what() << "variable " << *chunk << " not found!\n";
				exit(EXIT_FAILURE);
			}
		}

		// throw incorrect type error if not reserved (as to preserve arguments in function declarations)
		if (arg.type != call.typesWanted[i] && arg.type != StormType::RESERVE && arg.type != StormType::SVOID) {
			std::cerr << "Error: " << call.name << " expects " << (int)call.typesWanted[i] << " for argument " << i+1 << ". Got " << (int)arg.type << " instead.\n";
			exit(EXIT_FAILURE);
		}

		newText.push_back(0x0E);
		newText.push_back(0x10 + i);
		newText.insert(newText.end(), arg.ident.begin(), arg.ident.end());

		// get through comma
		chunk++;
	}
	for (uint8_t byte : newText) 
		parser.text.push_back(byte);

	parser.text.push_back(0x0A); // execute
}

void StormCall(std::vector<std::string>::iterator &chunk) {
	StormVMCall call = determineCall(*chunk);
	chunk++;
	// call will be added in this function
	addCallArgsToData(call, chunk);
}