/*
    storm_calls.h - structs and functions for storm_calls.cc
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

#include "../Storm/storm.h"

struct StormVMCall {
	std::vector<Type> typesWanted;
	std::string name;
	uint8_t byte;
};

// Read call
const StormVMCall STORM_READ = {{STRING, INTEGER}, "read", 0x40};
// Write call
const StormVMCall STORM_WRITE = {{STRING, STRING}, "write", 0x41};

// For calls to system functions (write, read, etc.) add args to data 
void addCallArgsToData(StormVMCall call, std::vector<std::string>::iterator &chunk);
// Execute Call
void StormCall(std::vector<std::string>::iterator &chunk);