#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef NTSTATUS(WINAPI* NtProtectVirtualMemory_t)(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID*           BaseAddress,
    IN OUT SIZE_T*          NumberOfBytesToProtect,
    IN ULONG                NewAccessProtection,
    OUT PULONG              OldAccessProtection
    );

int main(void) {
    HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
    NtProtectVirtualMemory_t NtProtectVirtualMemory = (NtProtectVirtualMemory_t)GetProcAddress(hNtDll, "NtProtectVirtualMemory");

    void * exec_mem;
    NTSTATUS rv;
    HANDLE th;
    DWORD oldprotect = 0;

    // 4 byte payload
    unsigned char payload[] = {
        0x90,      // NOP
        0x90,      // NOP
        0xcc,      // INT3
        0xc3       // RET
    };
    unsigned int payload_len = 4;

    // Allocate a memory buffer for payload
    exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    printf("%-20s : 0x%-016p\n", "exec_mem addr", (void *)exec_mem);
    printf("Current Permissions: PAGE_READWRITE\n");

    // Copy payload to new buffer
    RtlMoveMemory(exec_mem, payload, payload_len);

    // Make new buffer as executable
    SIZE_T size = payload_len;
    rv = NtProtectVirtualMemory(GetCurrentProcess(), &exec_mem, &size, PAGE_EXECUTE_READ, &oldprotect);
    printf("New Permissions: PAGE_EXECUTE_READ\n");

    printf("\nPress Any Key!\n");
    getchar();

    return 0;
}
