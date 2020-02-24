#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

inline struct program_t {
	std::string filename;
} program;

enum Type {
	INTEGER,
	STRING,
	BOOL
};

// return string without quotes
void stripString(std::string *quoted);