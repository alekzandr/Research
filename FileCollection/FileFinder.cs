using System;
using System.IO;
using System.Runtime.InteropServices;

namespace FileFinder
{
    class Program
    {
        // Import the necessary Windows API functions
        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr FindFirstFile(string lpFileName, out WIN32_FIND_DATA lpFindFileData);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern bool FindNextFile(IntPtr hFindFile, out WIN32_FIND_DATA lpFindFileData);

        [DllImport("kernel32.dll")]
        public static extern bool FindClose(IntPtr hFindFile);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern bool DeleteFile(string lpFileName);


        // Define the WIN32_FIND_DATA structure that will be used by the Windows API functions
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public struct WIN32_FIND_DATA
        {
            public uint dwFileAttributes;
            public System.Runtime.InteropServices.ComTypes.FILETIME ftCreationTime;
            public System.Runtime.InteropServices.ComTypes.FILETIME ftLastAccessTime;
            public System.Runtime.InteropServices.ComTypes.FILETIME ftLastWriteTime;
            public uint nFileSizeHigh;
            public uint nFileSizeLow;
            public uint dwReserved0;
            public uint dwReserved1;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string cFileName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 14)]
            public string cAlternateFileName;
        }

        static void Main(string[] args)
        {
            // Print the names of the Windows API functions that are being used
            Console.WriteLine("[+] Windows APIs");
            Console.WriteLine("[-] FindFirstFile");
            Console.WriteLine("[-] FindNextFile");
            Console.WriteLine("[-] FindClose");
            Console.WriteLine("[-] DeleteFile");
            Console.WriteLine("----------------------");
            Console.WriteLine("[?] Beginning File Collection...");


            // Get the filename and path from the command line arguments
            string filename = args[0];
            string path = args[1];

            // Get the sleep timer value from the command line arguments
            int sleepTimer = int.Parse(args[2]);

            // Construct the full path to the file by combining the path and filename
            string fullPath = Path.Combine(path, filename);

            // Use the Windows API to find the first matching file
            WIN32_FIND_DATA FindFileData;
            IntPtr findHandle = FindFirstFile(fullPath, out FindFileData);

            // Check if any matching files were found
            if (findHandle != IntPtr.Zero)
            {
                // Print the name of the first matching file
                Console.WriteLine(FindFileData.cFileName);

                // Delete the first matching file
                DeleteFile(FindFileData.cFileName);

                Console.WriteLine(FindFileData.cFileName + " - Deleted");

                // Loop through the remaining matching files
                while (FindNextFile(findHandle, out FindFileData))
                {
                    // Print the name of the next matching file
                    Console.WriteLine(FindFileData.cFileName + " - Not Deleted");

                    // Sleep for the specified amount of time
                    System.Threading.Thread.Sleep(sleepTimer);
                }

                // Close the handle to the search results
                FindClose(findHandle);
            }
            else
            {
                // No matching files were found, print an error message
                Console.WriteLine("No files were found matching the specified criteria.");
            }
        }
    }
}

