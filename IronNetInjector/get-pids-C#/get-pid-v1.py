"""
Filename: get-pid-v1.py
Procedure 1

This script leverages IronPython to make use of the .NET Framekwork.
Below we simply access the "Process" namespace and use the 
GetProcessesByName function. This function creates an array of 
new Process components and associates them with all the process 
resources on the local computer that share the specified process name.

References: 
- https://learn.microsoft.com/en-us/dotnet/api/system.diagnostics.process?view=net-7.0
- https://ironpython.net/documentation/dotnet/dotnet.html


"""

import sys
from System.Diagnostics import *

process_name = str(sys.argv[1])

def get_pid(process_name):
    return Process.GetProcessesByName(process_name)

procs = get_pid(process_name)

for proc in procs:
    print("Name: " + proc.ProcessName + "\n")
    print("    [-] Handle: " + str(proc.Handle) + "\n")
    print("    [-] PID: " + str(proc.Id) + "\n\n")
