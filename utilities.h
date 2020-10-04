#ifndef UTILITIES_H
#define UTILITIES_H 

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

namespace utilities {

/**
 * Print a collection as hexadecimal values.
 *
 * */
template <typename T>
inline void printToHex(T s)
{
	for(size_t i = 0; i < s.size(); i++) {
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)s[i];
	}
	std::cout << std::dec << "\n";
}

/**
 * Stream a collection as hexadecimal values.
 *
 * */
template <typename T>
inline std::ostream& streamToHex(T s, std::ostream& os)
{
	for(size_t i = 0; i < s.size(); i++) {
		os << std::hex << std::setfill('0') << std::setw(2) << (int)s[i];
	}
	os << std::dec;
	return os;
}
/**
 *
 * */
template <typename T>
inline std::ostream& printToHex(std::ostream& os, T s)
{
	for(size_t i = 0; i < s.size(); i++) {
		os << std::hex << std::setfill('0') << std::setw(2) << (int)s[i];
	}
	os << std::dec;
	return os;
}

/**
 * Print collection of bytes as a hexadecimal string.
 *
 * */
inline void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes)
{
	for (size_t i = 0; i < s.size(); i++) {
		if ((unsigned char)s[i] == 0xff) continue;
		hexBytes.push_back(s[i]);
	}
}

template <typename T>
inline void switchEndianness(T& collection)
{
	size_t i = 0, j = collection.size() - 1;
	while (i < collection.size() / 2) {
		auto tmp = collection[i];
		collection[i++] = collection[j];
		collection[j--] = tmp;
	}	
}

inline int hexDigitToInt(char digit)
{
	digit = tolower(digit);
	if (digit >= '0' && digit <='9')
	       return (int)(digit - '0');
	else if (digit >= 'a' && digit <= 'f') {
		return (int)(digit - '1' - '0') + 10; 
	}	
	return -1;
}

template <typename T>
inline int hexstringToBytes(std::string const&	hexstring, T& result)
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

template <typename T>
inline void bytesToHexstring(const T& bytes, std::string& s)
{
	std::stringstream ss;
	for (auto b : bytes) {
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
	}
	s = ss.str();
}

inline void bytesToDecimal(const std::vector<unsigned char>& bytes, std::string& result)
{
	size_t resultLen = bytes.size() * 3;
	std::vector<unsigned char> decimal(resultLen, 0);
	ssize_t resultIndex;

	for (size_t i = 0; i < bytes.size(); i++) {
		uint32_t carry = bytes[i];
		resultIndex = resultLen - 1;
		while (carry || resultIndex >= 0) {
			carry += 256 * decimal[resultIndex];
			decimal[resultIndex] = carry % 10;
			carry /= 10;
			resultIndex--;
		}
	}
	std::stringstream ss;
	bool leadingZero = true;
	for (auto el : decimal) {
		if (leadingZero && el == 0) {
			continue;
		}
		leadingZero = false;
		ss << (int)el;
	}
	result = ss.str();
}

} // utilities

#endif /* UTILITIES_H */
