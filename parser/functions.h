#pragma once

class function {
public:
	std::vector<uint8_t> ident;
	std::string name;

	// return type
	Type type;
	// args
	std::vector<Type> args;

	// total number declared
	static int TotalNumber;
	
	std::vector<uint8_t>::iterator loc;

	function(std::string _name);
};

// evaluate number of args given from function
int numArgsGiven(std::vector<std::string>::iterator chunk);
// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk);