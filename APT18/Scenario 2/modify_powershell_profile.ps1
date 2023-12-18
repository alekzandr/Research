# Check if the current user's PowerShell profile exists
if (!(Test-Path -Path $PROFILE)) {
    # Create the profile if it doesn't exist
    New-Item -ItemType File -Path $PROFILE -Force
}

# Script block to add to the profile
$scriptBlock = {
    if (Test-Connection -ComputerName google.com -Count 2 -Quiet) {
        Write-Host "Connectivity Achieved"
    } else {
        Write-Host "No Internet Connectivity"
    }
}

# Add the script block to the profile
$scriptBlockContent = $scriptBlock.ToString()
Add-Content -Path $PROFILE -Value $scriptBlockContent

Write-Host "PowerShell profile updated"
