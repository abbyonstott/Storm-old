/*
	storm.h - Includes, functions and structs used by the language
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