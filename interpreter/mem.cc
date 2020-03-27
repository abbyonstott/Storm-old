#include "../Storm/storm.h"
#include "interpreter.h"

// get location of identifier in its vector
int getLoc(std::string::iterator &loc, char endtoken) {
	loc++;
	std::vector<int> num_list;
	int num = 0;

	while (char(*loc - 0x80) != endtoken) {
		// get integer representation of digit in char
		num_list.push_back((int)(char(*loc - 0x80) - '0'));
		loc++;
	}

	for (int n = 0; n < num_list.size(); n++)
		num += num_list[n] * (n + 1);

	return num;
}

std::string getVal(std::string::iterator &loc) {
	std::string value;
	
	int num = getLoc(loc, ']');
	int i = interpreter.heap_ptrs[num];

	if (interpreter.heap[i] == (uint8_t)StormType::RESERVE) {
		std::cerr << "Error: tried to access unitialized variable at address " << num << '\n';
		exit(1);
	}
	else if (interpreter.heap[i] == '[' + 0x80) {
		// find the actual value. I'm sure there's a more efficient way to do this...
		std::string raw;
		while (i < interpreter.heap_ptrs[num + 1])
			raw += char(interpreter.heap[i++]);
		
		std::string::iterator rawloc = raw.begin();

		return getVal(rawloc);
	}

	while(i < interpreter.heap_ptrs[num + 1]) {
		value += ((interpreter.heap[interpreter.heap_ptrs[num]] > 0x7f) 
			? interpreter.heap[i] - 0x80 
			: interpreter.heap[i] + '0'
		);
		i++;
	}

	return value;
}

void callFunc(std::string::iterator &loc) {
	std::string::iterator locTemp = ++loc;

	loc = interpreter.functions[getLoc(loc, '}')];
	runScope(loc);

	loc = locTemp;
	while (char(*loc - 0x80) != '}') loc++;
}

// redefine var in heap, and change location pointers
void redefVar(int num, std::vector<uint8_t> data) {
	/* 
	 * The original size is difference between
	 * the location of the next value and the
	 * location of the value being changed
	*/
	int orig_size = interpreter.heap_ptrs[num + 1] - interpreter.heap_ptrs[num];

	// put bytecode into heap
	for (int i = interpreter.heap_ptrs[num]; i < interpreter.heap_ptrs[num + 1]; i++)
		interpreter.heap.erase(interpreter.heap.begin() + interpreter.heap_ptrs[num]);

	for (int i = 0; i < data.size(); i++) {
		interpreter.heap.insert(
			interpreter.heap.begin() + interpreter.heap_ptrs[num] + i,
			data.at(i)
		);
	}

	// adjust interpreter.heap_ptrs
	for (auto i = interpreter.heap_ptrs.begin() + num; i != interpreter.heap_ptrs.end(); i++)
		*i += data.size() - orig_size;

	interpreter.heap_ptrs[0] = 0;
}

void pushStack(std::string::iterator &loc) {
	
	
	std::string data = "";

	if (char(*(loc + 1) - 0x80) == '\"') {
		loc++;

		do {
			data += char(*(loc++) - 0x80);
		} while (char(*loc - 0x80) != '\"');
		
		data += char(*loc - 0x80);
	}
	else if (*(loc + 1) <= 0x09) {
		do {
			data += '0' + *(++loc);
		} while (*(loc + 1) <= 0x09);
	}
	else
		data = getVal(loc);

	interpreter.stack.push(data);
}

void popStack(std::string::iterator &loc) {
	loc++;
	int num = getLoc(loc, ']');

	// value on top of stack
	std::vector<uint8_t> sTop;

	for (char c : interpreter.stack.top())
		sTop.push_back(c + 0x80);
	
	interpreter.stack.pop();

	redefVar(num, sTop);
}

void createFunctions(std::string::iterator &loc) {
	loc++;
	interpreter.functions.resize(interpreter.functions.size() + 1);

	while (true) {
		int num = getLoc(loc, '}');
		
		if (num == 0) {
			interpreter.functions[0] = ++loc;
			break;
		}

		interpreter.functions.push_back(++loc);

		while (*loc != 0x19) loc++;
		loc++;
	}
}

void allocateMemory(std::string::iterator &loc) { // load data into memory
	loc++;

	while (*loc != 0x0B) {
		if (char(*loc - 0x80) == '[') {
			while (char(*loc - 0x80) != ']') loc++;
			loc++;
			interpreter.heap_ptrs.push_back(interpreter.heap.size());
		}

		// not unitialized
		if (*loc != (uint8_t)StormType::RESERVE) {
			// variable defined to be other variable
			if (char(*(loc + 1) - 0x80) == '[' && *loc != (uint8_t)StormType::SVOID) {
				loc++;
				for (char c : getVal(loc))
					interpreter.heap.push_back(c + 0x80);
			}
			else { // var defined as literal
				if (*loc == (uint8_t)StormType::SVOID)
					interpreter.heap.push_back(*(++loc));

				do {
					loc++;
					interpreter.heap.push_back(*loc);
				} while (char(*(loc + 1) - 0x80) != '[' && *(loc +1) != 0x0B);
			}
		}
		else {
			interpreter.heap.push_back(*loc);
		}

		loc++;
	}

	interpreter.heap_ptrs.push_back(interpreter.heap.size());
}