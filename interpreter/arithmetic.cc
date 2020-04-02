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

	for (int digit = value.size() - 1; digit >= 0; digit++)
		val += (value[digit] - '0') * pow(10, digit);

	return val;
}

void arithmeticOperation(std::string::iterator &loc) {
	MathOper operation = (MathOper)*loc;
	loc++;
	int source;

	// add to variable
	if (char(*(loc) - 0x80) == '[')
		source = std::stoi(getVal(loc));
	else { // to register
		int reg = *loc - 0x0F;

		source = std::stoi(interpreter.registers[reg]);
	}

	loc++;
	if (char(*(loc) - 0x80) == '[')
		source = std::stoi(getVal(loc));
	else if (*(loc) <= 9) {
		
	}
	else { // unlikely
		int reg = *loc - 0x0f;

		source = std::stoi(interpreter.registers[reg]);
	}

	switch (operation) {
		case MathOper::ADD:
			break;
		case MathOper::SUB:
			break;
		case MathOper::DIV:
			break;
		case MathOper::MULT:
			break;
	}
}