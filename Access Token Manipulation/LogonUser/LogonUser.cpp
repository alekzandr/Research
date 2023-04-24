#include <iostream>
#include <Windows.h>
#include <sddl.h>
#include <Ntsecapi.h>
#include <Lmcons.h> // Required for UNLEN
#include <chrono>
#include <thread>
#include <string>

void print_current_user() {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);
    std::wcout << L"Current user: " << username << std::endl;
    std::wcout << "[+] API Call: GetUserNameW" << std::endl;
}

void prompt_user_to_continue() {
    std::string input;

    std::cout << "Enter \"Yes\" to continue immediately, or wait for 1 minute to continue automatically: ";
    auto start = std::chrono::steady_clock::now();

    while (true) {
        if (std::cin.peek() != EOF) {
            std::cin >> input;
            if (input == "Yes") {
                std::cout << "Continuing immediately." << std::endl;
                break;
            }
            else {
                std::cout << "Invalid input. Enter \"Yes\" to continue immediately: ";
            }
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

        if (elapsed.count() >= 60) {
            std::cout << "1 minute has passed. Continuing automatically." << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
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

    prompt_user_to_continue();
    std::cout << "Continuing with the rest of the program." << std::endl;


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
