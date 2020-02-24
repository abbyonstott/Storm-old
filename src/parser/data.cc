#include "../storm.h"
#include "parser.h"

void changeByteVal() {
	std::vector<uint8_t> new_val_ident;
	char hval_ident[8];

	new_val_ident.push_back('[' + 0x80);

	// hex string representation of parser.val_ident
	sprintf(hval_ident, "%X", parser.val_ident); 
	std::string hval_ident_s = hval_ident;

	for (char n : hval_ident_s) {
		new_val_ident.push_back(n + 0x80);
	}

	new_val_ident.push_back(']' + 0x80);

	parser.byte_val_ident = new_val_ident;
}

void addArgToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);

	changeByteVal();
	parser.val_ident++;

	parser.data.insert(parser.data.end(), 
		parser.byte_val_ident.begin(), parser.byte_val_ident.end());

	// type byte
	parser.data.push_back(0x0D);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}