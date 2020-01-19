#include "storm.h"

void stripString(std::string *quoted) {
	quoted->erase(quoted->begin());
	quoted->pop_back();
}