#include "../storm.h"
#include "interpreter.h"

char evalEscape(std::vector<uint8_t>::iterator &byte) {
	char escChar;

	byte++;
	switch (*byte - 0x80)
	{
		case '\\':
			byte++;
			return '\\';
		case 'n':
			byte++;
			return '\n';
		default:
			byte++;
			return '\0';
	}
}

void write(std::vector<uint8_t>::iterator byte) {
	byte++;
	std::string contents, filename;

	// for now we can assume that the next byte is a string literal
	if (*byte == '\"' + 0x80) {
		byte++;

		// add bytes to contents until file ends
		while (*byte != '\"' + 0x80) {
			if (*byte - 0x80 != '\\') {
				contents += *(byte++) - 0x80;
			}
			else {
				contents += evalEscape(byte);
			}
		}
		
		byte += 2;

		while (*byte != '\"' + 0x80)
			filename += *(byte++) - 0x80;
	}
	
	std::ofstream file;
	file.open(filename);
	file << contents;
	file.close();
}