/*
	storm_calls.cc - parser for functions and their attributes
	Copyright (C) 2020 Ethan Onstott

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
	chunk += 2; // go to first argument

	// function has arguments
	if (*chunk != ")") {
		int given = numArgsGiven(chunk);
		
		// Wrong number of arguments?
		if (given != args.size()) {
			std::cerr << "Function " << name << " requires " << args.size() << " arguments " << given << " given!\n";
			exit(EXIT_FAILURE);
		}
	}

	
	for (variable& arg : args) {
		std::string argName = arg.name;
		
		if (*chunk == ",") chunk++;

		parser.text.push_back(0x0E);
		// identifier of the argument
		for (uint8_t byte : arg.ident)
			parser.text.push_back(byte);

		if ((*chunk)[0] == '\"') {
			// move [0], "literal"

			std::vector<uint8_t> bytecodeval = addStringToByteCode(*chunk);
			for (uint8_t byte : bytecodeval)
				parser.text.push_back(byte);
		}
		else if (isInt(*chunk)) {
			for (char c : *chunk)
				parser.text.push_back(c - '0');
		}
		else {
			try {
				for (uint8_t byte : find<variable>(*chunk).ident)
					parser.text.push_back(byte); 
			}
			catch(NameError& e) {
				std::cerr << e.what() << "name " << *chunk << " not found\n";
				exit(1);
			}
		}
		chunk++;
	}

	parser.text.push_back(0x1C); // call
	
	for (uint8_t byte : ident)
		parser.text.push_back(byte);
}

// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk, variable &v) {
	parser.data.push_back((uint8_t)StormType::RESERVE);

	// calls are handled differently than functions
	if (*chunk == "read") {
		/*
		 * running read and storing value in var:
		 *
		 * data
		 * 	[0] res
		 * 	[1] string "/dev/stdin"
		 * 	[2] integer 1
 		 * 
		 * text
		 * 	move reg0, 0x40 ; read
		 * 	move reg1, [1]  ; filename
		 * 	move reg2, [2] ; buffer size (1 in this case)
		 * 	exec ; move value of file in reg1 to reg3
		 * 	pop [0] ; take value and put into buffer
		*/
		StormCall(chunk);
		parser.text.push_back(0x18); // pop
		parser.text.insert(parser.text.end(),
			v.ident.begin(), v.ident.end());
		v.type = StormType::STRING;
	}
}

// evaluate number of args given from function
int numArgsGiven(std::vector<std::string>::iterator chunk) {
	int arg = 1;
	bool inquotes = 0;

	do {	
		if (*(chunk) == "," && inquotes == 0)
			arg++;

		chunk++;
	} while (*chunk != ")");

	return arg;
}