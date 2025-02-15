#include "lib.h"
#include <iostream>
#include <string>

void PrintUsage() {
    std::cerr
        << "Usage: get-clip [<filename>]\n"
        << "    If no filename is passed, the program will write to stdout.\n"
        << "    Use -h or --help to display this usage message.\n"
        << "    (Version 0.1)\n";
}

bool ParseArgs(int argc, char *argv[], Config &config) {
    if (argc > 2) {
        std::cerr << "Error: Incorrect number of arguments.\n";
        PrintUsage();
        return false;
    }
    config.mode = Mode::FROM_CLIPBOARD; // We are reading from the clipboard now

    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return false;
        }

        config.handle = CreateFileA(arg.c_str(), GENERIC_WRITE, 0, NULL,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (config.handle == INVALID_HANDLE_VALUE) {
            PrintLastWinError("Failed to open file " + arg);
            return false;
        }
    } else {
        // Use stdout if no filename is provided
        config.handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (config.handle == INVALID_HANDLE_VALUE) {
            PrintLastWinError("Failed to get standard handle");
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    Config config;
    if (!ParseArgs(argc, argv, config)) {
        return 1;
    }

    int res = Run(config);

    CleanupConfig(config);
    return res;
}
