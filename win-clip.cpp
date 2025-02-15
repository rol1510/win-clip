#include "lib.h"

void PrintUsage() {
    std::cerr << "Usage: program <options>\n"
              << "    -i <filepath or \"-\">   Read from <file> or stdin "
                 "(\"-\") and copy to "
                 "clipboard\n"
              << "    -o <filepath or \"-\">   Read from clipboard and write "
                 "to <file> or "
                 "stdout (\"-\")\n"
              << "    Use -i or -o but not both.\n"
              << "    (Version 0.1)\n";
}

bool ParseArgs(int argc, char *argv[], Config &config) {
    if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments.\n";
        PrintUsage();
        return false;
    }

    std::string option = argv[1];
    std::string value = argv[2];

    if (option == "-i") {
        config.mode = Mode::TO_CLIPBOARD;
    } else if (option == "-o") {
        config.mode = Mode::FROM_CLIPBOARD;
    } else {
        std::cerr << "Error: Unknown option '" << option << "'.\n";
        PrintUsage();
        return false;
    }

    if (value == "-") {
        config.handle = GetStdHandle((config.mode == Mode::TO_CLIPBOARD)
                                         ? STD_INPUT_HANDLE
                                         : STD_OUTPUT_HANDLE);
        if (config.handle == INVALID_HANDLE_VALUE) {
            PrintLastWinError("Failed to get standard handle");
            return false;
        }
    } else {
        config.handle = CreateFileA(
            value.c_str(),
            (config.mode == Mode::TO_CLIPBOARD) ? GENERIC_READ : GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            (config.mode == Mode::TO_CLIPBOARD) ? OPEN_EXISTING : CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        if (config.handle == INVALID_HANDLE_VALUE) {
            PrintLastWinError("Failed to open file " + value);
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
