#pragma once
#include <vector>
#include <string>

// Reads the IDAT section of a PNG file and returns the data.
std::vector<char> readIDAT(const std::string& filename);
