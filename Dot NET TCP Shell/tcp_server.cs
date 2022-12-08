using System;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;

namespace TcpServer
{
    class Program
    {
        static void Main(string[] args)
        {
            // Set the IP address of the remote host.
            IPAddress ipAddress = IPAddress.Parse("192.168.0.10");

            // Set the listening port on the remote host.
            int port = 8080;

            // Create a new TCP listener.
            TcpListener listener = new TcpListener(ipAddress, port);

            // Start listening for incoming connections.
            listener.Start();

            // Continuously listen for incoming connections and handle them.
            while (true)
            {
                // Accept an incoming connection.
                TcpClient client = listener.AcceptTcpClient();

                // Get the stream used to read and write data to the client.
                NetworkStream stream = client.GetStream();

                // Prompt the user to enter a command to send to the client.
                Console.Write("Enter a command to send to the client: ");
                string command = Console.ReadLine();

                // Send the command to the client.
                byte[] buffer = Encoding.UTF8.GetBytes(command);
                stream.Write(buffer, 0, buffer.Length);

                // Read the response from the client.
                buffer = new byte[1024];
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                // Print the response from the client.
                Console.WriteLine("Response from client: " + response);

                // Close the connection to the client.
                client.Close();
            }
        }
    }
}
