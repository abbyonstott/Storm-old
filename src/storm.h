#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>

inline struct program_t {
	std::string filename;

	// See docs/bytecode.md for list of all codes.
	std::vector<uint8_t> bytecode; 
} program;

enum Type {
	INTEGER,
	STRING,
	BOOL
};

// return string without quotes
void stripString(std::string *quoted);