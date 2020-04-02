#pragma once

class variable {
public:
	std::vector<uint8_t> ident;
	std::string name;

	StormType type;

	// total number declared
	static int TotalNumber;

    // overload to allow name to be declared with variable
    variable() = default;
	variable(std::string _name);

    ~variable() = default;
};

// declare variables
void declare(std::vector<std::string>::iterator &chunk, std::string name);
// add literal to data
void addLitToData(std::string literal);
// Convert std::string to bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit);
// evaluate mathematical expressions
void evalArithmetic(std::vector<std::string>::iterator &chunk, char operation);
std::vector<uint8_t> getRawValue(std::vector<std::string>::iterator chunk);
