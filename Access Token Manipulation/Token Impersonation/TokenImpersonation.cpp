#include <iostream>
#include <Windows.h>
#include <sddl.h>
#include <TlHelp32.h>
#include <Ntsecapi.h>
#include <Lmcons.h> // Required for UNLEN

void print_current_user() {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);
    std::wcout << L"Current user: " << username << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <PID>" << std::endl;
        return 1;
    }

    DWORD target_pid = static_cast<DWORD>(std::atoi(argv[1]));
    HANDLE target_process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, target_pid);
    if (!target_process) {
        std::cerr << "Error opening target process." << std::endl;
        return 2;
    }

    // Logging message
    std::wcout << L"[+] Using OpenProcess API" << std::endl;

    HANDLE target_token;
    if (!OpenProcessToken(target_process, TOKEN_DUPLICATE | TOKEN_QUERY | TOKEN_IMPERSONATE, &target_token)) {
        std::cerr << "Error opening target process token." << std::endl;
        CloseHandle(target_process);
        return 3;
    }

    // Logging message
    std::wcout << L"[+] Using OpenProcessToken API" << std::endl;

    HANDLE duplicated_token;
    if (!DuplicateTokenEx(target_token, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenPrimary, &duplicated_token)) {
        std::cerr << "Error duplicating token." << std::endl;
        CloseHandle(target_token);
        CloseHandle(target_process);
        return 4;
    }

    // Logging message
    std::wcout << L"[+] Using DuplicateTokenEx API" << std::endl;

    CloseHandle(target_token);
    CloseHandle(target_process);

    // Print current user
    print_current_user();

    // Impersonate the duplicated token
    if (!ImpersonateLoggedOnUser(duplicated_token)) {
        std::cerr << "Error impersonating logged on user." << std::endl;
        CloseHandle(duplicated_token);
        return 5;
    }

    // Logging message
    std::wcout << L"[+] Using ImpersonateLoggedOnUser API" << std::endl;

    // Print the user after impersonation
    print_current_user();

    // Revert to self
    if (!RevertToSelf()) {
        std::cerr << "Error reverting to self." << std::endl;
        CloseHandle(duplicated_token);
        return 6;
    }

    std::wcout << L"Using RevertToSelf API" << std::endl;

    // Print the user after reverting to self
    print_current_user();

    // Close the handles to avoid resource leaks
    CloseHandle(duplicated_token);

    return 0;
}
