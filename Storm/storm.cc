// don't add unnecessary includes from storm.h
#define STORM_MAIN_SOURCE

#include "storm.h"

/*
 Because Windows uses different functions to execute a process than POSIX, 
 there needs to be more than one main function.
*/
#ifndef _WIN32
int POSIX_main(int argc, char const* argv[]) {
	// if argv is not just the executable, then it must be run from "./command" 
	const bool local = !(std::string(argv[0]) == "storm");

	// status of execlp
	int run;
	std::string program;
	std::vector<char*> _args;

	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			// compile
			if (argc != 4) {
				std::cerr << "Command should be formatted as:\nstorm -c <file_name> <binary_name>\n";
				return EXIT_FAILURE;
			}

			program = ((local) ? "./stormcompiler" : "stormcompiler");
			_args.push_back((char*)program.c_str());
			_args.push_back((char*)argv[2]);
			_args.push_back((char*)argv[3]);
		}
		else if (argc == 2) {
			// run code
			program = ((local) ? "./stormrun" : "stormrun");
			_args.push_back((char*)program.c_str());
			_args.push_back((char*)argv[1]);
		}
	}
	else {
		std::cerr << "Format:\nstorm [options]...\n";
		return EXIT_FAILURE;
	}

	_args.push_back(NULL);

	// convert args to char *const array
	char* args[_args.size()];
	std::copy(_args.begin(), _args.end(), args);

	if (execvp(program.c_str(), args) == -1) {
		std::cerr << program << ": " << strerror(errno) << '\n';

		// no such file or directory on stormcompiler or stormrun
		if (errno == 2)
			std::cerr << "Error: install is broken. Core file " << program << " not found!\n";

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#else
int WIN32_main(int argc, char const* argv[]) {
	const bool local = !(std::string(argv[0]) == "storm.exe");
	STARTUPINFO startinfo;
	PROCESS_INFORMATION processinfo;

	SecureZeroMemory(&startinfo, sizeof(startinfo));
	startinfo.cb = sizeof(startinfo);
	SecureZeroMemory(&processinfo, sizeof(processinfo));

	std::string args, filename;

	if (argc >= 2) {
		if (std::string(argv[1]) == "-c") {
			if (argc != 4) {
				std::cerr << "Command should be formatted as:\nstorm -c <file_name> <binary_name>\n";
				return EXIT_FAILURE;
			}
			filename = "stormcompiler.exe ";

			args += filename;
			args += std::string(argv[2]);
			args += " ";
			args += std::string(argv[3]);
		}
		else if (argc == 2) {
			filename = "stormrun.exe ";
			args += filename;
			args += std::string(argv[1]);
		}
	}
	else {
		std::cerr << "Format:\nstorm [options]...\n";
		return EXIT_FAILURE;
	}

	// get the exit code of the process
	DWORD exitcode = 0;
	
	if (!CreateProcess(NULL,
		(LPSTR)args.c_str(),
		NULL,
		NULL,
		false,
		CREATE_NEW_PROCESS_GROUP,
		NULL,
		NULL,
		&startinfo,
		&processinfo))
	{
		std::cerr << "Error: process failed with errno: " << GetLastError() << '\n';
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			std::cerr << "install is broken. Core file " << filename << "not found!\n";
		}
		return EXIT_FAILURE;
	}
	WaitForSingleObject(processinfo.hProcess, INFINITE);
	if (GetExitCodeProcess(processinfo.hProcess, &exitcode) == 0) {
		std::cerr << "Couldn't get exit code\n";
		return EXIT_FAILURE;
	}
	if (exitcode > 0) {
		return EXIT_FAILURE;
	}

	CloseHandle(processinfo.hThread);
	CloseHandle(processinfo.hProcess);


	return EXIT_SUCCESS;
}
#endif // WIN32



int main(int argc, char const *argv[]) {
#ifndef _WIN32
	return POSIX_main(argc, argv);
#else
	return WIN32_main(argc, argv);
#endif // _WIN32
}