#pragma once

#include "../Storm/storm.h"

struct StormVMCall {
	std::vector<StormType> typesWanted;
	std::string name;
	uint8_t byte;
};

// Read call
const StormVMCall STORM_READ = {{StormType::STRING, StormType::INTEGER}, "read", 0x40};
// Write call
const StormVMCall STORM_WRITE = {{StormType::STRING, StormType::STRING}, "write", 0x41};
// Assert call assert(value, 0, 1, 2, value)
const StormVMCall STORM_ASSERT = {{StormType::SVOID, StormType::SVOID, StormType::BOOL}, "assert", 0x42};

// For calls to system functions (write, read, etc.) add args to data 
void addCallArgsToData(StormVMCall call, std::vector<std::string>::iterator &chunk);
// Execute Call
void StormCall(std::vector<std::string>::iterator &chunk);