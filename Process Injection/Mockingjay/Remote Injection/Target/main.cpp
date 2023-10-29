#include <Windows.h>
#include <iostream>

int main() {
    // Load the DLL into the current process
    HMODULE hMod = LoadLibrary(L"msys-2.0.dll");

    if (hMod == NULL) {
        std::cerr << "Failed to load msys-2.0.dll. Error code: " << GetLastError() << std::endl;
        return 1;
    }
    else {
        std::cout << "Successfully loaded msys-2.0.dll." << std::endl;
    }

    // Here you can use GetProcAddress() to find addresses of the functions in the DLL, etc.

    printf("\nHit me!\n");
    getchar();

    return 0;
}
