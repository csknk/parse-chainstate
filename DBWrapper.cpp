#include "DBWrapper.h"
#include "utilities.h"

DBWrapper::DBWrapper(std::string _dbName) : dbName(_dbName)
{
	std::cout << "constructor...\n";
	openDB();
	setObfuscationKey();
}

void DBWrapper::setObfuscationKey()
{
	std::cout << "set key...\n";
	obfuscationKeyKey = {0x0e , 0x00};
	obfuscationKeyKey += "obfuscate_key";
	std::string obfuscationKeyString;
	read(obfuscationKeyKey, obfuscationKeyString);
	utilities::stringToHexBytes(obfuscationKeyString, obfuscationKey);
	obfuscationKey.erase(obfuscationKey.begin());
	utilities::printToHex(obfuscationKey);
}

void DBWrapper::openDB()
{
	std::cout << "opening DB...\n";
	if (dbName.empty()) {
		throw std::invalid_argument{"No database specified in DBWrapper::openDB()"};
	}
	options.create_if_missing = false;
	leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
	readoptions.verify_checksums = true;
}

void DBWrapper::setDBName(const std::string& s)
{
	dbName = s;
}

void DBWrapper::read(std::string const& key, std::string& val)
{
	std::cout << "read...\n";
	status = db->Get(readoptions, key, &val);
	checkStatus("Error reading obfuscation key");
}

void DBWrapper::checkStatus(std::string msg)
{
	if (!status.ok()) {
		std::cerr << msg << "\n";
		std::cerr << status.ToString() << std::endl;
	}
	assert(status.ok());
	
}

void DBWrapper::outputAllKeyVals()
{
	leveldb::Iterator* it = db->NewIterator(readoptions);
	for (it->SeekToFirst(); it->Valid(); it->Next()) {

		std::vector<unsigned char> deObfuscatedKey;
		std::vector<unsigned char> deObfuscatedValue;
		deObfuscate(it->value(), deObfuscatedValue);
		std::cout << "KEY: ";
		std::vector<unsigned char> key;
		for (size_t i = 0; i < it->key().size(); i++) {
			key.push_back(it->key()[i]);
		} 
		utilities::printToHex(key);
		std::cout << "\nVALUE: ";
		utilities::printToHex(deObfuscatedValue);
		std::cout << "\n";
		if (key[0] == 0x43) {
			std::vector<unsigned char> txid;
			txid.insert(txid.begin(), key.begin() + 1, key.end() - 1);
			utilities::switchEndianness(txid);
			std::cout << "txid: ";
			utilities::printToHex(txid);
			std::cout << "\n";
	
		}
	}
	assert(it->status().ok());
	delete it;
}

void DBWrapper::fetchRecord(const std::string& key, std::string& value)
{
	std::string rawVal;
	status = db->Get(readoptions, key, &rawVal);
	if (!status.ok()) {
		throw std::invalid_argument("Key not found.");
	}
	std::vector<unsigned char> valBytes, deObBytes;
	utilities::stringToHexBytes(rawVal, valBytes);
	deObfuscate(valBytes, deObBytes);
	utilities::bytesToHexstring(deObBytes, value);	
}

/**
 * XOR `bytes` against the corresponding byte in the obfuscation key.
 * The obfuscation repeats as necessary.
 *
 * */
void DBWrapper::deObfuscate(leveldb::Slice bytes, std::vector<unsigned char>& plaintext)
{
	for (size_t i = 0, j = 0; i < bytes.size(); i++) {
		plaintext.push_back(obfuscationKey[j++] ^ bytes[i]);
		if (j == obfuscationKey.size()) j = 0;	
	}
}
