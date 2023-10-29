#include <iostream>

#include "Base64Helper.h"



int main() {

	std::string encodedMsiUrl = "aHR0cHM6Ly9naXRodWIuY29tL2FsZWt6YW5kci9SZXNlYXJjaC9JREFUL1VwZGF0ZXIubXNp"; // https://github.com/alekzandr/Research/IDAT/Updater.msi
	std::string decodedMsiUrl = Base64Decode(encodedMsiUrl);

	std::string encodedMsiFileName = "VXBkYXRlci5tc2k="; // Updater.msi
	std::string decodedMsiFileName = Base64Decode(encodedMsiFileName);

	std::cout << "Decoded: " << decodedMsiUrl << std::endl;
	std::cout << "Decoded: " << decodedMsiFileName << std::endl;

	return 0;

}

