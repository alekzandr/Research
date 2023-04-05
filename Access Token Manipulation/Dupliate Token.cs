using System;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace ProcessTokenRetriever
{
    class Program
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenProcess(uint processAccess, bool bInheritHandle, int processId);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool OpenProcessToken(IntPtr processHandle, uint desiredAccess, out IntPtr tokenHandle);

        [DllImport("kernel32.dll")]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool DuplicateTokenEx(
            IntPtr hExistingToken,
            uint dwDesiredAccess,
            IntPtr lpTokenAttributes,
            int impersonationLevel,
            int tokenType,
            out IntPtr phNewToken);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool ImpersonateLoggedOnUser(IntPtr hToken);

        const uint PROCESS_QUERY_INFORMATION = 0x0400;
        const uint TOKEN_QUERY = 0x0008;
        const uint TOKEN_DUPLICATE = 0x0002;
        const uint TOKEN_IMPERSONATE = 0x0004;
        const int SecurityImpersonation = 2;
        const int TokenType = 1;

        static void Main(string[] args)
        {
            // Get process handle
            IntPtr processHandle = GetProcessHandle(targetPid);

            if (processHandle != IntPtr.Zero)
            {
                // Get access token
                IntPtr accessToken = GetAccessToken(processHandle);

                if (accessToken != IntPtr.Zero)
                {
                    // Duplicate the access token
                    IntPtr duplicatedToken = DuplicateToken(accessToken);

                    if (duplicatedToken != IntPtr.Zero)
                    {
                        // Impersonate the logged-on user using the duplicated token
                        if (ImpersonateLoggedOnUser(duplicatedToken))
                        {
                            Console.WriteLine("Successfully impersonated the logged-on user.");
                            
                            // Perform actions on the system using the impersonated token
                            
                            // Revert to the original security context
                            RevertToSelf();
                        }
                        else
                        {
                            int errorCode = Marshal.GetLastWin32Error();
                            Console.WriteLine($"Failed to impersonate the logged-on user. Error code: {errorCode}");
                        }

                        // Close the duplicated token handle when done
                        CloseHandle(duplicatedToken);
                    }

                    // Close the access token handle when done
                    CloseHandle(accessToken);
                }

                // Close the process handle when done
                CloseHandle(processHandle);
            }
        }

        static IntPtr GetProcessHandle(int targetPid)
        {
            Console.WriteLine($"Attempting to obtain a handle for process with PID: {targetPid}");

            IntPtr processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, false, targetPid);
            if (processHandle == IntPtr.Zero)
            {
                int errorCode = Marshal.GetLastWin32Error();
                Console.WriteLine($"Failed to obtain a handle for process with PID {targetPid}. Error code: {errorCode}");
            }
            else
            {
                Console.WriteLine($"Successfully obtained a handle for process with PID {targetPid}. Handle: {processHandle}");
            }

            return processHandle;
        }

        static IntPtr GetAccessToken(IntPtr processHandle)
        {
            Console.WriteLine($"Attempting to obtain the access token for process with handle: {processHandle}");

            if (OpenProcessToken(processHandle, TOKEN_QUERY, out IntPtr accessToken))
            {
                return accessToken;
            }
            else
            {
                int errorCode = Marshal.GetLastWin32Error();
                Console.WriteLine($"Failed to obtain the access token for process with handle {processHandle}. Error code: {errorCode}");
                return IntPtr.Zero;
            }
        }

        static IntPtr DuplicateToken(IntPtr accessToken)
        {
            Console.WriteLine($"Attempting to duplicate the access token: {accessToken}");

            if (DuplicateTokenEx(
                accessToken,
                TOKEN_IMPERSONATE | TOKEN_DUPLICATE,
                IntPtr.Zero,
                SecurityImpersonation,
                TokenType,
                out IntPtr duplicatedToken))
            {
                Console.WriteLine($"Successfully duplicated the access token. Duplicated token: {duplicatedToken}");
                return duplicatedToken;
            }
            else
            {
                int errorCode = Marshal.GetLastWin32Error();
                Console.WriteLine($"Failed to duplicate the access token. Error code: {errorCode}");
                return IntPtr.Zero;
            }
        }

    }
}
