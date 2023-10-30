#include "PNGReader.h"
#include <iostream>
#include <fstream>

std::vector<char> readIDAT(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> idatData;

    if (!file) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return idatData;
    }

    // Check PNG signature
    char signature[8];
    file.read(signature, 8);
    if (memcmp(signature, "\x89PNG\x0D\x0A\x1A\x0A", 8) != 0) {
        std::cerr << "Not a PNG file: " << filename << std::endl;
        return idatData;
    }

    while (file) {
        // Read chunk length and type
        uint32_t length;
        char type[4];
        file.read(reinterpret_cast<char*>(&length), 4);
        file.read(type, 4);

        // Convert to little-endian
        length = __builtin_bswap32(length);

        if (std::string(type, 4) == "IDAT") {
            // Read IDAT data
            idatData.resize(length);
            file.read(idatData.data(), length);
            break;
        }
        else {
            // Skip data and CRC
            file.seekg(length + 4, std::ios::cur);
        }
    }

    return idatData;
}
