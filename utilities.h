#ifndef UTILITIES_H
#define UTILITIES_H 

#include <iostream>
#include <iomanip>

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
}
} // utilities

#endif /* UTILITIES_H */
