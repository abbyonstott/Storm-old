#include "../Storm/storm.h"
#include "parser.h"

// similar to variable constructor, uses {} instead of []
function::function(std::string _name) {
	name = _name;

	char hval_ident[8];

	// hex form of hval
#ifndef WIN32
	sprintf(hval_ident, "%X", TotalNumber);
#else
	sprintf_s(hval_ident, "%X", TotalNumber);
#endif // WIN32


	ident.push_back('{' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back('}' + 0x80);

	TotalNumber++;
}

// find function by name and return ident
template<>
function find<function>(std::string name) {
	for (function f : parser.functions) {
		if (f.name == name)
			return f;
	}
	throw NameError();
}

void create_func(std::vector<std::string>::iterator &chunk, function *funcObj) {
	// store parser.text in a tempvar to get only the function text
	std::vector<uint8_t> tempText = parser.text;
	std::vector<variable> tempVars = parser.vars;
	// The location of the function in tempText
	std::vector<uint8_t>::iterator loc;

	parser.text = {};

	// set value of loc to after last function
	loc = ((parser.functions.size() == 2) 
		? tempText.begin()
		// get the last value's offset and add it to loc
		: tempText.begin() + parser.functions[parser.functions.size() - 2].offset);

	chunk+=2;
	// start on first arg, go until closing parentheses
	while (*chunk != ")") {
		if (*chunk != ",") {
			funcObj->args.push_back(variable(*chunk));
			funcObj->args.back().type = StormType::RESERVE;

			parser.data.push_back((uint8_t)StormType::RESERVE);
			parser.vars.push_back(funcObj->args.back());

			// add nameless reference to tempvars to avoid repetition of variable idents
			tempVars.push_back(funcObj->args.back());
			tempVars.back().name = "";
		}

		chunk++;
	}

	// check if function already defined
	if (find<function>(funcObj->name).ident != funcObj->ident) {
		std::cerr << NameError().what() << "Function " << funcObj->name << " already defined.\n";
		exit(EXIT_FAILURE);
	}

	for (uint8_t byte : funcObj->ident)
		parser.text.push_back(byte);

	funcObj->type = StormType::SVOID;
	// skip to first line of function
	while (*chunk != "{") chunk++;
	// and finally, parse
	funcObj->parse(++chunk);

	// really doesn't matter for first defined
	funcObj->offset = parser.text.size() + parser.functions[parser.functions.size() - 2].offset;

	// add the function into the original text
	tempText.insert(loc, parser.text.begin(), parser.text.end());
	parser.text = tempText;
	parser.vars = tempVars;
}

void function::run(std::vector<std::string>::iterator &chunk) {
	/*
	 * How the function should look in bytecode when run.
	 * data
	 * 	[0] string "value" ; already-defined variable
	 * 	[1] res ; first argument of function {1}
	 * text
	 * {1}
	 *	; do something
	 *	ret
	 * {0}
	 *	; if literal
	 * 	move [0], "literal" ; move value of literal to [0]
	 *	;
	 *
	 *	; if value
	 *	move [0], [1] ; move value of [1] into our first argument
	 *	;
	 *
	 *	call {1}
	 *	exit
	*/
	int given = 0;

	chunk += 2; // move to )

	// function has arguments
	if (*chunk != ")")
		given = numArgsGiven(chunk - 1); // starts at (

	if (args.size() != given) {
		std::cerr << "Function " << name << " requires " << args.size() << " argument(s). " << given << " given!\n";
		exit(EXIT_FAILURE);
	}

	/* 
	 * adding to vector is delayed due to inline 
	 * functions with return values also having their 
	 * own output
	*/
	std::vector<uint8_t> newText;

	for (variable& arg : args) {
		std::string argName = arg.name;
		
		
		if (*chunk == ",") chunk++;

		if ((*chunk)[0] == '\"') {
			newText.push_back(0x0E);
			for (uint8_t byte : arg.ident)
				newText.push_back(byte);

			// move [0], "literal"
			std::vector<uint8_t> bytecodeval = addStringToByteCode(*chunk);
			for (uint8_t byte : bytecodeval)
				newText.push_back(byte);
		}
		else if (isInt(*chunk)) {
			newText.push_back(0x0E);
			for (uint8_t byte : arg.ident)
				newText.push_back(byte);

			for (char c : *chunk)
				newText.push_back(c - '0');
		}
		else if (*(chunk + 1) == "(") {
			// inline function
			variable v;
			inlineFunc(chunk, v);
			chunk++;
		
			newText.push_back(0x18); // pop
			// the identifier of the argument
			for (uint8_t byte : arg.ident)
				newText.push_back(byte);
			continue;
		}
		else {
			try {
				newText.push_back(0x0E);
				// the identifier of the argument
				for (uint8_t byte : arg.ident)
					newText.push_back(byte);

				for (uint8_t byte : find<variable>(*chunk).ident)
					newText.push_back(byte); 
			}
			catch(NameError& e) {
				std::cerr << e.what() << "variable " << *chunk << " not found\n";
				exit(1);
			}
		}
		chunk++;
	}

	// add newtext to parser.text
	for (uint8_t byte: newText)
		parser.text.push_back(byte);

	parser.text.push_back(0x1C); // call
	
	for (uint8_t byte : ident)
		parser.text.push_back(byte);
}

void function::returnValue(std::vector<std::string>::iterator &chunk) {
	// main has no return value
	if (name == "main") {
		std::cerr << "Error: no returns in main scope";
		exit(EXIT_FAILURE);
	}

	StormType retType;

	try {
		variable rVal = find<variable>(*(++chunk));
		
		retType = rVal.type;

		parser.text.push_back(0x17);
		// ident if var
		for (uint8_t byte : rVal.ident)
			parser.text.push_back(byte);
	}
	catch (NameError &n) {
		// literal (or function)
		if ((*chunk)[0] == '\"') {
			parser.text.push_back(0x17);
			for (uint8_t byte : addStringToByteCode(*chunk))
				parser.text.push_back(byte);

			retType = StormType::STRING;
		}
		else if (isInt(*chunk)) { // int literal
			parser.text.push_back(0x17);
			for (char c : *chunk)
				parser.text.push_back(c - '0');

			retType = StormType::INTEGER;
		}
		else if (*(chunk + 1) == "(") { // inline func
			variable rVal;
			// if it has a return type push value
			inlineFunc(chunk, rVal);
			chunk++;
			
			if (rVal.type == StormType::SVOID) {
				// Error out if function is void
				std::cerr << "Error: Function in return of " << name << " cannot be void!\n";
				exit(1);
			}

			retType = rVal.type;
		}
		else {
			std::cerr << n.what() << *chunk << " is not a recognized identifier.\n";
			exit(1);	
		}
	}

	// if not first time, make sure the types are the same.
	if (type == StormType::SVOID)
		type = retType;
	else if (type != retType) {
		std::cerr << "Error: wrong return type in " << name << '\n';
		exit(1);
	}
}

// assign value to function
void inlineFunc(std::vector<std::string>::iterator &chunk, variable &v) {
	if (*chunk == "read") {
		StormCall(chunk);
		v.type = StormType::STRING;
		chunk--; // subtract one to stay at the same location as the normal functions do.
	}
	else { // normal function

		function f;
		// check if function exists
		try {
			f = find<function>(*chunk);
		}
		catch(NameError &n) {
			std::cerr << n.what() << "function " << *chunk << " not found.\n";
			exit(1);
		}
		
		f.run(chunk);

		v.type = f.type;
	}
}

/*
 * evaluate number of args given from function
 * starts at the (
*/
int numArgsGiven(std::vector<std::string>::iterator chunk) {
	int arg = 1, inlines = 0;

	chunk++; // get past ( to allow inlines to be properly captured
	do {	
		if (*(chunk) == "," && inlines == 0)
			arg++;
		else if (*chunk == "(")
			inlines++;
		else if (*chunk == ")" && inlines != 0)// inline
			inlines--;
		chunk++;
	} while (*chunk != ")" || inlines != 0);

	return arg;
}