#include "DBWrapper.h"

DBWrapper::DBWrapper()
{
	setObfuscationKey();
	std::cout << utilities::printHex(obfuscationKeyKey) << "\n";	
}

DBWrapper::setObfuscationKey()
{
	obfuscationKeyKey = {0x0e, 0x00} + "obfuscate_key";
}
