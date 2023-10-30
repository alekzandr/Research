#include "APIHasher.h"

unsigned int hash_api(const char* name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash << 7) + (hash >> 25) + *name;
        name++;
    }
    return hash;
}

void* get_api_address_by_hash(HMODULE module, unsigned int hash) {
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)module + dosHeader->e_lfanew);
    IMAGE_OPTIONAL_HEADER optionalHeader = ntHeaders->OptionalHeader;
    IMAGE_EXPORT_DIRECTORY* exportDirectory = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)module + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    ULONG* addressOfFunctions = (ULONG*)((BYTE*)module + exportDirectory->AddressOfFunctions);
    ULONG* addressOfNames = (ULONG*)((BYTE*)module + exportDirectory->AddressOfNames);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)module + exportDirectory->AddressOfNameOrdinals);

    for (DWORD i = 0; i < exportDirectory->NumberOfNames; i++) {
        char* functionName = (char*)((BYTE*)module + addressOfNames[i]);
        if (hash_api(functionName) == hash) {
            return (void*)((BYTE*)module + addressOfFunctions[addressOfNameOrdinals[i]]);
        }
    }

    return nullptr;

}
