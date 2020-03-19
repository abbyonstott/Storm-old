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

	std::cerr << "Error: function " << name << " not found!\n";
	exit(EXIT_FAILURE);
}

void create_func(std::vector<std::string>::iterator &chunk, function *funcObj) {
	while (*chunk != "{")
		chunk++;

	std::vector<uint8_t> tempText = parser.text;
	parser.text = {};

	// for now, will add to it after defined. Kept like this to add back to parser.text when finished
	funcObj->loc = ((parser.functions.size() == 2) ? tempText.begin() : (parser.functions.end() - 1)->loc); 
	funcObj->type = StormType::SVOID;

	for (uint8_t byte : funcObj->ident)
		parser.text.push_back(byte);

	funcObj->parse(chunk);

	tempText.insert((funcObj->loc + 1), parser.text.begin(), parser.text.end());
	parser.text = tempText;
}

// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk, variable &v) {
	parser.data.push_back(0x15); // res

	// calls are handled differently than functions
	if (*chunk == "read") {
		/*
		running read and storing value in var:
		
		data
			[0] res
			[1] string "/dev/stdin"
			[2] integer 1

		text
			move reg0, 0x40 ; read
			move reg1, [1]  ; filename
			move reg2, [2] ; buffer size (1 in this case)
			exec ; move value of file in reg1 to reg3
			pop [0] ; take value and put into buffer
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
	} while (*chunk != "]");

	return arg;
}

/*
Example: Print Function

Storm:

func print[buf] {
	write(buf, "/dev/stdout");
}

print["Hello, World!"];


Bytecode Assembly:

data
	[0] string "/dev/stdout"
	[1] string "Hello, World!\n"

text
{1} ; print function
	pop reg1 pop argument 1 (buf) off stack into reg1

	move reg0, write
	mov reg2, [0] ; location (/dev/stdout)
	
	exec
	
	ret

{0}
	push [1]
	call {1} ; print["Hello, World!\n"];
	exit
*/