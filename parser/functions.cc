#include "../Storm/storm.h"
#include "parser.h"

// similar to variable constructor, uses {} instead of []
function::function(std::string _name) {
	name = _name;

	char hval_ident[8];

	// hex form of hval
	sprintf(hval_ident, "%X", TotalNumber);

	ident.push_back('{' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back('}' + 0x80);

	TotalNumber++;
}

// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk) {
	variable v("");
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