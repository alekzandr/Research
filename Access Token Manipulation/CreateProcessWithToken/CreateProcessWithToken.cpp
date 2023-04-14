#include <iostream>
#include <Windows.h>
#include <sddl.h>
#include <TlHelp32.h>
#include <Ntsecapi.h>

void print_privileges(HANDLE token) {
    DWORD privileges_size = 0;
    GetTokenInformation(token, TokenPrivileges, nullptr, 0, &privileges_size);
    PTOKEN_PRIVILEGES privileges = reinterpret_cast<PTOKEN_PRIVILEGES>(malloc(privileges_size));
    GetTokenInformation(token, TokenPrivileges, privileges, privileges_size, &privileges_size);

    for (DWORD i = 0; i < privileges->PrivilegeCount; ++i) {
        LUID_AND_ATTRIBUTES& la = privileges->Privileges[i];
        LSA_UNICODE_STRING lsa_string;
        DWORD name_length = 0;
        if (!LookupPrivilegeNameW(nullptr, &la.Luid, nullptr, &name_length)) {
            DWORD error_code = GetLastError();
            if (error_code == ERROR_INSUFFICIENT_BUFFER) {
                lsa_string.Buffer = reinterpret_cast<PWSTR>(malloc((name_length + 1) * sizeof(WCHAR)));
                lsa_string.Length = name_length * sizeof(WCHAR);
                lsa_string.MaximumLength = (name_length + 1) * sizeof(WCHAR);
                LookupPrivilegeNameW(nullptr, &la.Luid, lsa_string.Buffer, &name_length);
            }
        }

        std::wcout << L"Privilege: " << lsa_string.Buffer << L" - " << ((la.Attributes & SE_PRIVILEGE_ENABLED) ? L"Enabled" : L"Disabled") << std::endl;
        free(lsa_string.Buffer);
    }
    free(privileges);
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

    HANDLE target_token;
    if (!OpenProcessToken(target_process, TOKEN_DUPLICATE | TOKEN_QUERY, &target_token)) {
        std::cerr << "Error opening target process token." << std::endl;
        CloseHandle(target_process);
        return 3;
    }

    std::wcout << L"Current privileges:" << std::endl;
    print_privileges(target_token);

    HANDLE duplicated_token;
    if (!DuplicateTokenEx(target_token, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenPrimary, &duplicated_token)) {
        std::cerr << "Error duplicating token." << std::endl;
        CloseHandle(target_token);
        CloseHandle(target_process);
        return 4;
    }

    CloseHandle(target_token);
    CloseHandle(target_process);

    wchar_t notepad_path[] = L"C:\\Windows\\System32\\notepad.exe";

    //wchar_t command_line[] = L"shell:AppsFolder\\Microsoft.WindowsCalculator_8wekyb3d8bbwe!App";

    STARTUPINFO startup_info = { 0 };
    PROCESS_INFORMATION process_info = { 0 };
    startup_info.cb = sizeof(startup_info);

    if (!CreateProcessWithTokenW(duplicated_token, 0, notepad_path, nullptr, 0, nullptr, nullptr, &startup_info, &process_info)) {
        std::cerr << "Error creating process with duplicated token." << std::endl;
        CloseHandle(duplicated_token);
        return 5;
    }

    std::wcout << L"Successfully created notepad.exe with the duplicated token." << std::endl;

    // Close the handles to avoid resource leaks
    CloseHandle(duplicated_token);
    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);

    return 0;
}


