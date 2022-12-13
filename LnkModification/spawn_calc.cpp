//#include <iostream>
//#include <stdlib.h> // for system() function
//#pragma comment(lib, "stdlib.lib")
//#include <windows.h>

#include <iostream>
#include <stdlib.h> // for system() function

int main() {
	// Use system() function to open calc.exe
	// The system() function takes a string as argument,
	// which contains the command we want to execute.
	// In this case, the command is "calc.exe"
	system("C:\\Windows\\System32\\calc.exe");

	// You can also use the full path to calc.exe,
	// if it is not in the same directory as your program.
	// For example:
	// system("C:\\Windows\\System32\\calc.exe");

	return 0;
}