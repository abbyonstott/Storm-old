#include "../Storm/storm.h"
#include "interpreter.h"
#include "arithmetic.h"
#include "math.h"

/*
 * Takes location in file and returns the full integer
 * value.
 * Ex.
 * Given a string of length n -> 
 * val = (str[n - 1] * 10^n-1) + (str[n-2] * 10^n-2) + ...
*/
int getIntFromLoc(std::string::iterator &loc) {
	int val = 0;
	std::string value;
	
	while (*loc <= 9) {
		value += *(loc++);
	}

	for (int digit = value.size() - 1; digit >= 0; digit--)
		val += value[digit] * pow(10, digit);

	return val;
}

/*
 * Convert an integer to a bytecode string
*/
std::vector<uint8_t> getIntBytecode(int val) {
	std::vector<uint8_t> buf;
	std::string StringVersion = std::to_string(val);

	for (char c : StringVersion)
		buf.push_back(c - '0');
	
	return buf;
}

void arithmeticOperation(std::string::iterator &loc) {
	MathOper operation = (MathOper)*loc;
	loc++;
	bool reg = false;
	int var_num, reg_num;
	int destination, source;

	// add to variable
	if (char(*(loc) - 0x80) == '[') {
		std::string::iterator getLoc_Location = loc;
		var_num = getLoc(getLoc_Location, ']');

		destination = std::stoi(getVal(loc));
	}
	else { // to register
		reg = true;
		reg_num = *loc - 0x0F;

		destination = std::stoi(interpreter.registers[reg_num]);
	}

	loc++;
	if (char(*(loc) - 0x80) == '[')
		source = std::stoi(getVal(loc));
	else if (*(loc) <= 9)
		source = getIntFromLoc(loc);
	else { // unlikely
		int reg = *loc - 0x0f;

		source = std::stoi(interpreter.registers[reg]);
	}

	switch (operation) {
		case MathOper::ADD:
			destination += source;
			break;
		case MathOper::SUB:
			destination += source;
			break;
		case MathOper::DIV:
			destination /= source;
			break;
		case MathOper::MULT:
			destination *= source;
			break;
	}

	if (reg)
		interpreter.registers[reg_num] = std::to_string(destination);
	else
		redefVar(var_num, getIntBytecode(destination));
}