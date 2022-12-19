# Get System IP Address
$ip_address = Get-NetIPAddress -AddressFamily IPv4 | Where-Object {$_.IPAddress -notlike "169.254*"} | Select-Object IPAddress -First 1
$ip_address = $ip_address.IPAddress

# Start TCP Server
$serverProcess = Start-Process .\tcp_server.exe $ip_address -PassThru

# Wait for the process to start
Start-Sleep -Seconds 3

# Start the TCP Client program
$clientProcess = Start-Process .\TCP_Reverse_Shell.exe $ip_address -PassThru

# Wait for the process to start
Start-Sleep -Seconds 3

# Sleep so user can enter a command
start-sleep 10

# Stop running processes
Stop-Process -id $serverProcess.Id
Stop-Process -id $clientProcess.Id
