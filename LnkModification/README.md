# LNK Modification

## Description
This binary looks for the Firefox.lnk file in the current directory. It then changes the path and arguments of the shortcut to use `cmd.exe` to open the `spawn_calc.exe` payload. After running `lnk_mod.exe`, double click the **Firefox** shortcut in the folder and a **Calculator** process should spawn.

## Steps
1. Run `lnk_mod.exe`
2. Double click the Firefox shortcut