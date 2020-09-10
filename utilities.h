#ifndef UTILITIES_H
#define UTILITIES_H 

#include <iostream>
#include <iomanip>
#include <vector>

namespace utilities {

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
	std::cout << "\n";
}

/**
 * Print collection of bytes as a hexadecimal string.
 *
 * */
void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes)
{
	for (size_t i = 0; i < s.size(); i++) {
		if ((unsigned char)s[i] == 0xff) continue;
		hexBytes.push_back(s[i]);
	}
}

template <typename T>
void switchEndianness(T& collection)
{
	size_t i = 0, j = collection.size() - 1;
	while (i < collection.size() / 2) {
		auto tmp = collection[i];
		collection[i++] = collection[j];
		collection[j--] = tmp;
	}	
}

int hexDigitToInt(char digit)
{
	digit = tolower(digit);
	if (digit >= '0' && digit <='9')
	       return (int)(digit - '0');
	else if (digit >= 'a' && digit <= 'f') {
		return (int)(digit - '1' - '0') + 10; 
	}	
	return -1;
}

int hexstringToBytes(std::string const&	hexstring, std::vector<unsigned char>& result)
{
	if (hexstring.size() % 2) {
		std::cerr << "The hexstring is not an even number of characters.\n";
		exit(EXIT_FAILURE);
	}
	
	size_t resultLength = hexstring.size() / 2;
	size_t i = 0;
	for (auto it = hexstring.begin(); it != hexstring.end(); it = it + 2) {
		int sixteens = hexDigitToInt(*it);
		int units = hexDigitToInt(*std::next(it));
		if (units == -1 || sixteens == -1) {
			std::cerr << "Invalid hex character.\n";
			exit(EXIT_FAILURE);
		}
		result.push_back((sixteens << 4) | units);
		i++;
	}
	return resultLength;
}

void bytesToHexstring(const std::vector<unsigned char>& bytes, std::string& s)
{
	std::stringstream ss;
	for (auto b : bytes) {
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
	}
	s = ss.str();
}

} // utilities

#endif /* UTILITIES_H */
