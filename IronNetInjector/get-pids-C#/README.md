# C# Method

Here we utilize the C# method by using the `GetProcessesName` method. `GetProcessesName` returns an array of new Process components and associates them with all the process resources on the local computer that share the specified process name. From the array we print the `Name`, `Handle`, and `Id` properties.

![Output](Output.JPG)


## Requirements
- [IronPython 2.7](https://ironpython.net/)

## Usage:
To access the .NET Framework, we use the IronPython interpreter to execute the python scripts. In our case, we've already set the PATH variables to allow this.

```ipy.exe get-pid-v1.py explorer```

## Warning
This script has no form of error handling and the most basic of argument processing. There are no intentions to make it more robust or secure. Use at your discretion.
