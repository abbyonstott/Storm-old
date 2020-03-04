/*
    svalues.h - Struct for svalues.cc
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

inline struct svalues_t
{
	// next value identifier
	unsigned int val_ident = 0;

	// bytecode representation of the last value identifier used
	std::vector<uint8_t> byte_val_ident;
    	std::vector<std::string> names;
} svalues;