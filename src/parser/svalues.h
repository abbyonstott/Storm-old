#pragma once

inline struct svalues_t
{
	// next value identifier
	unsigned int val_ident = 0;

	// bytecode representation of the last value identifier used
	std::vector<uint8_t> byte_val_ident;
    	std::vector<std::string> names;
} svalues;