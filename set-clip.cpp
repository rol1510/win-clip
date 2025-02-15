#include "lib.h"
#include <iostream>
#include <string>

void PrintUsage() {
    std::cerr
        << "Usage: set-clip [<filename>]\n"
        << "    If no filename is passed, the program will read from stdin.\n"
        << "    Use -h or --help to display this usage message.\n"
        << "    (Version 0.1)\n";
}

bool ParseArgs(int argc, char *argv[], Config &config) {
    if (argc > 2) {
        std::cerr << "Error: Incorrect number of arguments.\n";
        PrintUsage();
        return false;
    }
    config.mode = Mode::TO_CLIPBOARD;

    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return false;
        }

        config.handle =
            CreateFileA(arg.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (config.handle == INVALID_HANDLE_VALUE) {
            PrintLastWinError("Failed to open file " + arg);
            return false;
        }
    } else {
        config.handle = GetStdHandle(STD_INPUT_HANDLE);
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
