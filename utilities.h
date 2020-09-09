#ifndef UTILITIES_H
#define UTILITIES_H 

#include <iostream>
#include <iomanip>
#include <vector>

namespace utilities {

template <typename T> void printToHex(T s);
void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes); 

/**
 * Print a collection as hexadecimal values.
 *
 * */
template <typename T>
void printToHex(T s)
{
	for(size_t i = 0; i < s.size(); i++) {
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)s[i];
	}
}

void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes)
{
	for (size_t i = 0; i < s.size(); i++) {
		if ((unsigned char)s[i] == 0xff) continue;
		hexBytes.push_back(s[i]);
	}
}
} // utilities

#endif /* UTILITIES_H */
