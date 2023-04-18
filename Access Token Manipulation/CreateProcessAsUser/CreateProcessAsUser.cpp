#include <iostream>
#include <Windows.h>
#include <sddl.h>
#include <TlHelp32.h>
#include <Ntsecapi.h>
#include <UserEnv.h>
#include <Aclapi.h>


void print_privileges(HANDLE token) {
    DWORD privileges_size = 0;
    GetTokenInformation(token, TokenPrivileges, nullptr, 0, &privileges_size);
    PTOKEN_PRIVILEGES privileges = reinterpret_cast<PTOKEN_PRIVILEGES>(malloc(privileges_size));
    GetTokenInformation(token, TokenPrivileges, privileges, privileges_size, &privileges_size);

    for (DWORD i = 0; i < privileges->PrivilegeCount; ++i) {
        LUID_AND_ATTRIBUTES& la = privileges->Privileges[i];
        LSA_UNICODE_STRING lsa_string = { 0 };
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

bool enable_privileges(HANDLE token) {
    // Dynamically allocate TOKEN_PRIVILEGES structure with enough space for two LUID_AND_ATTRIBUTES
    DWORD tp_size = sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES);
    PTOKEN_PRIVILEGES token_privileges = reinterpret_cast<PTOKEN_PRIVILEGES>(malloc(tp_size));
    ZeroMemory(token_privileges, tp_size);
    token_privileges->PrivilegeCount = 2;

    token_privileges->Privileges[0] = { 0 };
    token_privileges->Privileges[1] = { 0 };

    if (!LookupPrivilegeValueW(nullptr, SE_ASSIGNPRIMARYTOKEN_NAME, &token_privileges->Privileges[0].Luid) ||
        !LookupPrivilegeValueW(nullptr, SE_INCREASE_QUOTA_NAME, &token_privileges->Privileges[1].Luid)) {
        free(token_privileges);
        return false;
    }

    token_privileges->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    token_privileges->Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

    BOOL result = AdjustTokenPrivileges(token, FALSE, token_privileges, 0, nullptr, nullptr);
    free(token_privileges);

    return result;
}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <PID>" << std::endl;
        return 1;
    }

    DWORD target_pid = static_cast<DWORD>(std::atoi(argv[1]));
    std::wcout << L"API Call: OpenProcess" << std::endl;
    HANDLE target_process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, target_pid);
    if (!target_process) {
        std::cerr << "Error opening target process." << std::endl;
        return 2;
    }

    std::wcout << L"API Call: OpenProcessToken" << std::endl;
    HANDLE target_token;
    if (!OpenProcessToken(target_process, TOKEN_DUPLICATE | TOKEN_QUERY, &target_token)) {
        std::cerr << "Error opening target process token." << std::endl;
        CloseHandle(target_process);
        return 3;
    }

    std::wcout << L"Current privileges:" << std::endl;
    print_privileges(target_token);

    std::wcout << L"API Call: DuplicateTokenEx" << std::endl;
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

    STARTUPINFOW startup_info = { 0 };
    PROCESS_INFORMATION process_info = { 0 };
    startup_info.cb = sizeof(startup_info);


    
    HANDLE current_process = GetCurrentProcess();
    HANDLE current_token;
    if (!OpenProcessToken(current_process, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &current_token)) {
        std::cerr << "Error opening current process token." << std::endl;
        return 6;
    }

    if (!enable_privileges(current_token)) {
        std::cerr << "Error enabling privileges." << std::endl;
        CloseHandle(current_token);
        return 7;
    }
    

    // New Block
    // Enable the required privileges for the current process
        HANDLE current_process_token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &current_process_token)) {
        std::cerr << "Error opening current process token." << std::endl;
        return 6;
    }

    LUID luid;
    if (!LookupPrivilegeValue(nullptr, SE_ASSIGNPRIMARYTOKEN_NAME, &luid)) {
        std::cerr << "Error looking up SeAssignPrimaryTokenPrivilege." << std::endl;
        CloseHandle(current_process_token);
        return 7;
    }

    TOKEN_PRIVILEGES token_privileges;
    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = luid;
    token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(current_process_token, FALSE, &token_privileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        std::cerr << "Error enabling SeAssignPrimaryTokenPrivilege." << std::endl;
        CloseHandle(current_process_token);
        return 8;
    }

    if (!LookupPrivilegeValue(nullptr, SE_INCREASE_QUOTA_NAME, &luid)) {
        std::cerr << "Error looking up SeIncreaseQuotaPrivilege." << std::endl;
        CloseHandle(current_process_token);
        return 9;
    }

    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = luid;
    token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(current_process_token, FALSE, &token_privileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        std::cerr << "Error enabling SeIncreaseQuotaPrivilege." << std::endl;
        CloseHandle(current_process_token);
        return 10;
    }

    // End of new block

    std::wcout << L"API Call: CreateProcessAsUserW" << std::endl;
    if (!CreateProcessAsUserW(duplicated_token, notepad_path, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup_info, &process_info)) {
        DWORD error_code = GetLastError();
        LPWSTR error_message = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&error_message),
            0,
            nullptr);

        std::wcerr << L"Error creating process with duplicated token. Error Code: " << error_code << L" - " << error_message << std::endl;
        LocalFree(error_message);
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
