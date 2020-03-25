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