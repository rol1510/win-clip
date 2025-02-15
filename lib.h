#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <stdio.h>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

enum class Mode {
    TO_CLIPBOARD,
    FROM_CLIPBOARD,
};

struct Config {
    Mode mode;
    HANDLE handle;
};

std::wstring ConvertUtf8ToWide(const std::string &str) {
    int count =
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}

std::string ConvertWideToUtf8(const std::wstring &wstr) {
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(),
                                    NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL,
                        NULL);
    return str;
}

bool WriteUtf8ToClipboard(std::string str) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }

    bool success = true;

    EmptyClipboard();

    std::wstring wstr = ConvertUtf8ToWide(str);

    size_t size = (wstr.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);

    if (hMem) {
        WCHAR *pMem = (WCHAR *)GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, wstr.c_str(), size);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        } else {
            success = false;
        }
    } else {
        success = false;
    }

    CloseClipboard();
    return success;
}

bool ReadUtf8FromClipboard(std::string &text) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return false;
    }

    WCHAR *pText = static_cast<WCHAR *>(GlobalLock(hData));
    if (pText == nullptr) {
        CloseClipboard();
        return false;
    }

    std::wstring wstr(pText);

    GlobalUnlock(hData);
    CloseClipboard();

    text = ConvertWideToUtf8(wstr);
    return true;
}

std::string ReadString(HANDLE handle) {
    std::string input;
    const DWORD CHUNK_SIZE = 4096;
    char buffer[CHUNK_SIZE];
    DWORD bytesRead;

    while (true) {
        if (!ReadFile(handle, buffer, CHUNK_SIZE, &bytesRead, nullptr) ||
            bytesRead == 0) {
            break;
        }
        input.append(buffer, bytesRead);
    }

    return input;
}

bool WriteString(HANDLE handle, const std::string &text) {
    DWORD bytesWritten;
    if (!WriteFile(handle, text.c_str(), text.size(), &bytesWritten, nullptr)) {
        return false;
    }

    return true;
}

void PrintLastWinError(const std::string &message) {
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return;

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, NULL);

    std::cerr << message << ": " << messageBuffer << std::endl;
    LocalFree(messageBuffer);
}

void CleanupConfig(Config &config) {
    if (config.handle != INVALID_HANDLE_VALUE &&
        config.handle != GetStdHandle(STD_INPUT_HANDLE) &&
        config.handle != GetStdHandle(STD_OUTPUT_HANDLE)) {
        CloseHandle(config.handle);
        config.handle = INVALID_HANDLE_VALUE;
    }
}

int Run(Config &config) {
    if (config.mode == Mode::TO_CLIPBOARD) {
        std::string input = ReadString(config.handle);
        if (WriteUtf8ToClipboard(input) == false) {
            PrintLastWinError("Failed to write to clipboard");
            return 1;
        }
    } else if (config.mode == Mode::FROM_CLIPBOARD) {
        std::string clipboardText;
        if (ReadUtf8FromClipboard(clipboardText) == false) {
            PrintLastWinError("Failed to read from clipboard");
            return 1;
        }

        if (WriteString(config.handle, clipboardText) == false) {
            PrintLastWinError("Failed to write to file");
            return 1;
        }
    } else {
        throw std::runtime_error("Unhandled enum value!");
    }
    return 0;
}

#endif // LIB_H
