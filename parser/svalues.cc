#include "../Storm/storm.h"
#include "parser.h"

/*
 * variable class constructor
 * set name
 * create ident
*/
variable::variable(std::string _name) {
	name = _name;

	char hval_ident[8];
	// get hex of var
#ifndef WIN32
	sprintf(hval_ident, "%X", TotalNumber);
#else
	sprintf_s(hval_ident, "%X", TotalNumber);
#endif // WIN32

	ident.push_back('[' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back(']' + 0x80);

	// add identifier to data
	parser.data.insert(parser.data.end(), 
		ident.begin(), ident.end());

	TotalNumber++;
}

// Convert std::string to bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}

// find var by name and return ident
template<>
variable find<variable>(std::string name) {
	for (variable v : parser.vars) {
		if (v.name == name && v.name != "")
			return v;
	}

	throw NameError();
}

void addLitToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}

// declare variable
void declare(std::vector<std::string>::iterator &chunk, std::string name) {
	variable v(name);

	if (name != "")
		chunk += 2;

	if ((*chunk)[0] == '\"') {// string literal
		v.type = StormType::STRING;
		parser.data.push_back((int)v.type);
		
		addLitToData(*chunk);
	}
	else if (isInt(*chunk)) {
		v.type = StormType::INTEGER;

		parser.data.push_back((int)v.type);
		
		// push_back value of int
		for (char c : *chunk)
			parser.data.push_back(c - '0');
	}
	else if (*(chunk+1) == "(") { // function
		// assigned later
		parser.data.push_back((uint8_t)StormType::RESERVE);
		inlineFunc(chunk, v);
		// pop into variable
		parser.text.push_back(0x18);
		parser.text.insert(parser.text.end(),
			v.ident.begin(), v.ident.end());

		parser.vars.push_back(v);

		while (*chunk != ";") chunk++;
		
		return;
	}
	else {
		// search for already defined variable
		variable *match;

		try {
			match = new variable(find<variable>(*chunk));
		}
		catch (NameError& e) {
			std::cerr << e.what() << "variable " << *chunk << " not found!\n";
			exit(EXIT_FAILURE);
		}
		
		v.type = (match->type != StormType::RESERVE) 
			? match->type
			: StormType::SVOID;
		
		parser.data.push_back((int)v.type);
		parser.data.insert(parser.data.end(),
			match->ident.begin(), match->ident.end());
		
		delete match;
	}

	parser.vars.push_back(v);

	if (name != "")
		chunk++;
}