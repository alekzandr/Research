using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EnumWindows
{
    public class Class1
    {

        public delegate bool EnumedWindow(IntPtr handleWindow, int param);


        // DLL Imports

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool EnumWindows(EnumedWindow lpEnumFunc, int lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int GetWindowText(IntPtr hWnd, StringBuilder title, int size);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool IsWindowVisible(IntPtr hWnd);
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.U4)] //Original .bool
        static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

        public struct WindowInfo
        {
            // Turla Struct for saving Visible Window Data

            public string ProcessName;
            public string Caption;
            public int PID;

            public void GetInfo()

                // Method for writing open windows to console
            {
                Console.WriteLine("Process: " + ProcessName);
                Console.WriteLine("Caption: " + Caption);
                Console.WriteLine("PID: " + PID);
                Console.WriteLine("\n");
                //return true;
            }
        }



        static public bool GetWindowsInformation(IntPtr window, int param)
        {
            /**
            * Turla Function to get window information.
            * Based on method_235
            **/
            if (!IsWindowVisible(window)) // Pass over non-visible windows
            {
                return true;
            }

            // Declare and initilizae struc 
            WindowInfo windowInfo = new WindowInfo();
            windowInfo.ProcessName = "";
            windowInfo.PID = 0;
            StringBuilder text = new StringBuilder();

            // Get PID
            GetWindowThreadProcessId(window, out windowInfo.PID);

            // Get Window Text
            GetWindowText(window, text, text.Capacity);


            windowInfo.ProcessName = Process.GetProcessById((int)windowInfo.PID).ProcessName;
            windowInfo.Caption = text.ToString();
            windowInfo.GetInfo();
            return true;
        }

        static void EnumerateWindows()
        {
            // Turla function using EnumWindows
            // Gets information and writes to console

            //WindowInfo window = new WindowInfo();
            //StringBuilder windowText = new StringBuilder(255);
            //int pid = 0;
            EnumedWindow callBackPtr = GetWindowsInformation;
            EnumWindows(callBackPtr, 0);
        }

        public static void Main(string[] args)
        {
            Console.WriteLine("Opened Windows");
            EnumerateWindows();
        }

    }
}
