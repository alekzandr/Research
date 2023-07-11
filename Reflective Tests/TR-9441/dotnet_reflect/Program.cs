using System;
using System.IO;
using System.Reflection;

class Program
{
    static void Main()
    {
        // Read the DLL file into a byte array
        byte[] bytes = File.ReadAllBytes("MyAssembly.dll");

        // Load the assembly from the byte array
        Assembly assembly = Assembly.Load(bytes);

        // Get the type and method to execute
        Type type = assembly.GetType("myNamespace.MyClass");
        MethodInfo method = type.GetMethod("MyMethod");

        // Create an instance of the type
        object obj = Activator.CreateInstance(type);

        // Check if obj is null
        if (obj == null)
        {
            Console.WriteLine("Failed to create an instance of " + type);
        }
        else
        {
            // Invoke the method on the instance
            method.Invoke(obj, null);
        }
    }
}
