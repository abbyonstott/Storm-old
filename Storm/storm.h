#pragma once

// includes for everything
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <string>
#include <cstring>
#include <assert.h>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#include <io.h>

#define WIN32_LEAN_AND_MEAN
#else
#include <unistd.h>
#endif // _WIN32

#ifndef STORM_MAIN_SOURCE
// these are not included in the main source file to save resources
#include <fstream>
#include <algorithm>

inline struct program_t {
	std::string filename;
} program;


// Types and their bytecodes
enum class StormType {
	// for functions and references to non typed arguments
	SVOID = 0x1D,

	/*
	 * in parser:
	 * Use parser.data.push_back((uint8_t)StormType::TYPE);
	*/
	INTEGER = 0x14,
	STRING = 0x0D,
	BOOL = 0x1B,
	RESERVE = 0x15
};

// Arithmetic operation
enum class MathOper {
	ADD = 0x1E,
	SUB = 0x1F,
	MULT = 0x20,
	DIV = 0x21
};

// Something was referenced that doesn't exist
struct NameError : public std::exception {
	const char * what() const throw() {
		return "NameError: ";
	}
};

// return string without quotes
void stripString(std::string *quoted);
// determine if string contains int
bool isInt(std::string str);

#endif // STORM_MAIN_SOURCE