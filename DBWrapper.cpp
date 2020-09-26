#include "DBWrapper.h"
#include "utilities.h"

DBWrapper::DBWrapper(std::string _dbName) : dbName(_dbName)
{
	openDB();
	setObfuscationKey();
}

DBWrapper::~DBWrapper()
{
	delete db;
}

void DBWrapper::setObfuscationKey()
{
	// Build the key needed to fetch the obfuscation key: `obfuscationKeyKey`.
	// This is stored as a value in the database.
	obfuscationKeyKey = {0x0e , 0x00};
	obfuscationKeyKey += "obfuscate_key";
	
	std::string obfuscationKeyString;
	read(obfuscationKeyKey, obfuscationKeyString);
	utilities::stringToHexBytes(obfuscationKeyString, obfuscationKey);

	// The first character of the obfuscationKey is 0x08 and should be removed.
	obfuscationKey.erase(obfuscationKey.begin());
	utilities::printToHex(obfuscationKey);
}

void DBWrapper::openDB()
{
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

void DBWrapper::fetchRecord(const std::string& txid, std::vector<unsigned char>& value)
{
	std::vector<char> keyBytes;
	utilities::hexstringToBytes(txid, keyBytes);
	utilities::switchEndianness(keyBytes);
	keyBytes.insert(keyBytes.begin(), 0x43);
	keyBytes.insert(keyBytes.end(), 0x00); // vout
	leveldb::Slice keySlice(keyBytes.data(), keyBytes.size());
	std::string rawVal;
	
	status = db->Get(readoptions, keySlice, &rawVal);
		if (!status.ok()) {
		throw std::invalid_argument("Key not found.");
	}
	
	deObfuscate(rawVal, value);
}

void DBWrapper::fetchRecord(const std::string& txid, std::string& value)
{
	std::vector<unsigned char> deObBytes;
	fetchRecord(txid, deObBytes);
	utilities::bytesToHexstring(deObBytes, value);
}
