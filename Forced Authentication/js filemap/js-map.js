var network = new ActiveXObject("WScript.Network");
var fso = new ActiveXObject("Scripting.FileSystemObject");

// Map a network drive (G:) to the remote share
network.MapNetworkDrive("G:", "\\\\192.168.50.224\\files");

// Specify the file path on the mapped drive
var filePath = "G:\\example.txt";

// Check if the file exists
if (fso.FileExists(filePath)) {
    // Open the file for reading
    var file = fso.OpenTextFile(filePath, 1);
    var fileContent = file.ReadAll();
    file.Close();

    // Display the file content
    WScript.Echo(fileContent);
} else {
    WScript.Echo("File does not exist.");
}

// Disconnect the mapped drive
network.RemoveNetworkDrive("G:");
