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

/*
 * return the bytecode representation of a given 
 * value
*/
std::vector<uint8_t> getRawValue(std::vector<std::string>::iterator chunk) {
	std::vector<uint8_t> value;

	if ((*chunk)[0] == '\"')
		value = addStringToByteCode(*chunk);
	else if (isInt(*chunk)) {
		for (char c : *chunk)
			value.push_back(c - '0');
	} 
	else if (*chunk == "true" || *chunk == "false")
		value.push_back((*chunk == "true"));
	else {
		try {
			value = find<variable>(*chunk).ident;
		}
		catch (NameError &n) {
			std::cerr << n.what() << *chunk << " is not a value\n";
		}
	}

	return value;
}

void evalArithmetic(std::vector<std::string>::iterator &chunk, char operation) {
	variable v;

	try {
		v = find<variable>(*chunk);	
	}
	catch (NameError &n) {
		std::cerr << n.what() << *chunk << " is not a recognized variable\n";
		exit(1);
	}

	// get past operator
	chunk += 2;
	if (chunk->size() != 1) {
		/*
		 * User used an operator as an assignment operator that should
		 * be an arithmetic operator
		 * e.g. + instead of +=
		*/
		std::cerr << "Error: no operator '" << operation << " in current context.\n";
		exit(1);
	}
	std::vector<uint8_t> right;

	switch ((*chunk)[0]) {
		case '+':
			if (operation != '+') {
				std::cerr << "Error: " << *chunk << operation << " is not a valid operator";
				exit(0);
			}

			parser.text.push_back(0x1E);
			right.push_back(1);
			break;
		case '-':
			if (operation != '-') {
				std::cerr << "Error: " << *chunk << operation << " is not a valid operator";
				exit(0);
			}
			
			parser.text.push_back(0x1F); // sub
			right.push_back(1);
			break;
		case '=': // += -= etc
			if (operation == '+')
				parser.text.push_back(0x1E);
			else if (operation == '-')
				parser.text.push_back(0x1F);
			else if (operation == '*')
				parser.text.push_back(0x20);
			else // operation == '/' 
				parser.text.push_back(0x21);

			chunk++;
			right = getRawValue(chunk);
			break;
	}

	parser.text.insert(parser.text.end(), v.ident.begin(),  v.ident.end());
	parser.text.insert(parser.text.end(), right.begin(),  right.end());
	chunk++;
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

	/*
	 * If a variable's name is blank, it means that 
	 * it is a temporary variable (typically used for
	 * function arguments) and is not stored properly 
	 * in memory.
	*/
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
	else if ((*chunk) == "true" || *(chunk) == "false") {
		v.type = StormType::BOOL;
		parser.data.push_back(0x1B);

		parser.data.push_back(
			(*chunk == "true") ? 1 : 0
		); // bool values are stored as integers
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
