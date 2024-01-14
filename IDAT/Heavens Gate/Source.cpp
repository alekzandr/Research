#include <stdint.h>
#include <windows.h>

#include "HeavensGate.h"

int main()
{	
    // Load kernel32.dll
    uint64_t kernel32 = GetModuleHandle64(L"kernel32.dll");

    // Get the address of CreateProcessA
    uint64_t CreateProcessA = GetProcAddress64(kernel32, (uint64_t)"CreateProcessA");

    // Define the command line string (name of the program to be executed)
    char commandLine[] = "calc.exe";

    // Call CreateProcessA
    // Arguments for CreateProcessA are:
    // 1. lpApplicationName - NULL, since we are specifying the executable in the command line
    // 2. lpCommandLine - Command line string (should be writable)
    // 3. lpProcessAttributes - NULL
    // 4. lpThreadAttributes - NULL
    // 5. bInheritHandles - FALSE
    // 6. dwCreationFlags - 0
    // 7. lpEnvironment - NULL
    // 8. lpCurrentDirectory - NULL
    // 9. lpStartupInfo - Pointer to STARTUPINFO (needs to be properly initialized)
    // 10. lpProcessInformation - Pointer to PROCESS_INFORMATION (receives information about the new process)

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Note: The addresses of 'si' and 'pi' must be passed to the x64 context.
    // This may require some adjustments or a different approach depending on how X64Call handles memory addresses.

    X64Call(CreateProcessA, 10,
        (uint64_t)0, // lpApplicationName
        (uint64_t)commandLine, // lpCommandLine
        (uint64_t)0, // lpProcessAttributes
        (uint64_t)0, // lpThreadAttributes
        (uint64_t)FALSE, // bInheritHandles
        (uint64_t)0, // dwCreationFlags
        (uint64_t)0, // lpEnvironment
        (uint64_t)0, // lpCurrentDirectory
        (uint64_t)&si, // lpStartupInfo
        (uint64_t)&pi // lpProcessInformation
    );

    // Optional: Close process and thread handles
    // This should be done in the x64 context if necessary.

    return 0;
}