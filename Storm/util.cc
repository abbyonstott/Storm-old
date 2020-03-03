/*
    util.cc - Useful functions for the parser or interpreter
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
#include "storm.h"

void stripString(std::string *quoted) {
	quoted->erase(quoted->begin());
	quoted->pop_back();
}

bool isInt(std::string str) {
	for (char c : str) {
		if (!isdigit(c))
			return false;
	}	

	return true;
}