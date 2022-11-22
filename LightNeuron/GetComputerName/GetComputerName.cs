using System;
using System.Runtime.InteropServices;
using System.Text;

namespace GetComputerName
{
    class Program
    {
        enum COMPUTER_NAME_FORMAT
        {
            ComputerNameNetBIOS,
            ComputerNameDnsHostname,
            ComputerNameDnsDomain,
            ComputerNameDnsFullyQualified,
            ComputerNamePhysicalNetBIOS,
            ComputerNamePhysicalDnsHostname,
            ComputerNamePhysicalDnsDomain,
            ComputerNamePhysicalDnsFullyQualified
        }

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern bool GetComputerNameEx(COMPUTER_NAME_FORMAT NameType,
            StringBuilder lpBuffer, ref uint lpnSize);

        [STAThread]
        static void Main(string[] args)
        {
            bool success;
            StringBuilder name = new StringBuilder(260);
            uint size = 260;
            success = GetComputerNameEx(COMPUTER_NAME_FORMAT.ComputerNameDnsHostname, name, ref size);
            Console.WriteLine(name.ToString());
        }
    }
}
