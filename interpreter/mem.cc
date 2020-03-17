/*
    mem.cc - interpreter memory handling
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
#include "../Storm/storm.h"
#include "interpreter.h"

void allocateMemory(std::string::iterator &loc) { // load data into memory
	loc++;

	while (*loc != 0x0B) {
		if (char(*loc - 0x80) == '[') {
			while (char(*loc - 0x80) != ']') loc++;
			loc++;
			interpreter.heap_ptrs.push_back(interpreter.heap.size());
		}

		// not unitialized
		if (*loc != 0x15) {
			// variable defined to be other variable
			if (char(*loc - 0x80) == '[') {
				loc++;
				for (char c : getVal(loc))
					interpreter.heap.push_back(c + 0x80);
			}
			else { // var defined as literal
				do {
					loc++;
					interpreter.heap.push_back(*loc);
				} while (char(*(loc + 1) - 0x80) != '[' && *(loc +1) != 0x0B);
			}
		}
		else {
			interpreter.heap.push_back(0x15);
		}

		loc++;
	}

	interpreter.heap_ptrs.push_back(interpreter.heap.size());
}