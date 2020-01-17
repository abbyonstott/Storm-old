#include "stormscript.h"
#include "file/file.h"

int main(int argc, char const *argv[]) {
    std::string fname;

    if (argc >= 2)
        fname = argv[1];
    else {
        std::cerr << "Expected filename\n";
        return -1;
    }
    

    std::cout << fname << '\n';

    return 0;
}