#ifndef DBWRAPPER_H
#define DBWRAPPER_H 

#include <iostream>
#include <vector>
#include "utilities.h"

class DBWrapper {
private:
	const std::string obfuscationKeyKey;
	std::vector<unsigned char> obfuscationKey;

public:
	DBWrapper();
	void setObfuscationKey();
};


#endif /* DBWRAPPER_H */
