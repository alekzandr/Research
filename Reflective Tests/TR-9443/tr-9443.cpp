#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef NTSTATUS(WINAPI* NtAllocateVirtualMemory_t)(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID*           BaseAddress,
    IN ULONG_PTR            ZeroBits,
    IN OUT PSIZE_T          RegionSize,
    IN ULONG                AllocationType,
    IN ULONG                Protect
    );

int main(void) {
    HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
    NtAllocateVirtualMemory_t NtAllocateVirtualMemory = (NtAllocateVirtualMemory_t)GetProcAddress(hNtDll, "NtAllocateVirtualMemory");

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
    SIZE_T size = payload_len;

    // Allocate a memory buffer for payload
    PVOID baseAddress = NULL;
    NtAllocateVirtualMemory(GetCurrentProcess(), &baseAddress, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    exec_mem = baseAddress;
    printf("%-20s : 0x%-016p\n", "exec_mem addr", (void *)exec_mem);
    printf("Current Permissions: PAGE_EXECUTE_READWRITE\n");

    printf("\nPress Any Key!\n");
    getchar();

    return 0;
}
