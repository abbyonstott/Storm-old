#pragma once

class function {
public:
	std::vector<uint8_t> ident;
	std::string name;

	// return type
	StormType type;
	// args
	std::vector<variable> args;

	// total number declared
	static int TotalNumber;
	
	/*
	 * The function's text size + the offset of the last one
	 * or the sum off all text sizes for each function
	 * 
	 * If this is the first defined, parser.functions[parser.functions.size() - 2].offset
	 * is 0
	*/
	int offset;

	function() = default;
	function(std::string _name);

	void parse(std::vector<std::string>::iterator &chunk);

	~function() = default;

	void run(std::vector<std::string>::iterator &chunk);
};

// evaluate number of args given from function
int numArgsGiven(std::vector<std::string>::iterator chunk);
// run function that is inside of value
void inlineFunc(std::vector<std::string>::iterator &chunk, variable &v);
// create function
void create_func(std::vector<std::string>::iterator &chunk, function *funcObj);