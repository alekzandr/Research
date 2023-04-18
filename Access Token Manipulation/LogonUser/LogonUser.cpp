#include <iostream>
#include <Windows.h>
#include <sddl.h>
#include <Ntsecapi.h>
#include <Lmcons.h> // Required for UNLEN

void print_current_user() {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);
    std::wcout << L"Current user: " << username << std::endl;
    std::wcout << "[+] API Call: GetUserNameW" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <Username> <Password>" << std::endl;
        return 1;
    }

    wchar_t* username = new wchar_t[UNLEN + 1];
    size_t converted_chars = 0;
    mbstowcs_s(&converted_chars, username, UNLEN + 1, argv[1], _TRUNCATE);

    wchar_t* password = new wchar_t[UNLEN + 1];
    converted_chars = 0;
    mbstowcs_s(&converted_chars, password, UNLEN + 1, argv[2], _TRUNCATE);

    HANDLE logon_token;
    if (!LogonUserW(
        username,
        nullptr, // Use the local computer
        password,
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &logon_token)) {
        std::cerr << "Error logging on user." << std::endl;
        return 2;
    }
    std::wcout << "[+] API Call: LogonUserW" << std::endl;

    // Print current user
    print_current_user();

    // Impersonate the logon token
    if (!ImpersonateLoggedOnUser(logon_token)) {
        std::cerr << "Error impersonating logged on user." << std::endl;
        CloseHandle(logon_token);
        return 3;
    }
    std::wcout << "[+] API Call: ImpersonateLoggedOnUser" << std::endl;

    // Print the user after impersonation
    print_current_user();

    // Revert to self
    if (!RevertToSelf()) {
        std::cerr << "Error reverting to self." << std::endl;
        CloseHandle(logon_token);
        return 4;
    }
    std::wcout << "[+] API Call: RevertToSelf" << std::endl;

    // Print the user after reverting to self
    print_current_user();

    // Close the handles to avoid resource leaks
    CloseHandle(logon_token);

    delete[] username;
    delete[] password;

    return 0;
}
