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

/**
 * Build the key needed to fetch the obfuscation key: `obfuscationKeyKey`.
 *
 * This is stored as a value in the database with the byte values {0x0e, 0x00}
 * prepended to the string "obfuscate_key".
 * The first character of the retrieved obfuscationKey is 0x08 and should be removed.
 * */
void DBWrapper::setObfuscationKey()
{
	obfuscationKeyKey = {0x0e, 0x00};
	obfuscationKeyKey += "obfuscate_key";
	std::string obfuscationKeyString;
	read(obfuscationKeyKey, obfuscationKeyString);
	utilities::stringToHexBytes(obfuscationKeyString, obfuscationKey);
	obfuscationKey.erase(obfuscationKey.begin());
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

/**
 * The first 32 bytes of txidvout is txid, the remaining bytes are a varint representing the vout. 
 *
 * */
void DBWrapper::outputAllKeyVals(std::tuple<BytesVec, BytesVec, BytesVec> result) 
{
	leveldb::Iterator* it = db->NewIterator(readoptions);
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		BytesVec deObfuscatedKey;
		BytesVec deObfuscatedValue;
		deObfuscate(it->value(), deObfuscatedValue);
		std::cout << "KEY: ";
		BytesVec key;
		for (size_t i = 0; i < it->key().size(); i++) {
			key.push_back(it->key()[i]);
		} 
		utilities::printToHex(key);
		std::cout << "\nVALUE: ";
		utilities::printToHex(deObfuscatedValue);
		std::cout << "\n";
		if (key[0] == 0x43) {
			BytesVec txid;
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

/**
 * Fetch a record from the LevelDB database chainstate.
 *
 * The key should be a little-endian representation of the txid bytes, prepended by the byte 0x43
 * and appended with the value of the vout of the specific UTXO.
 *
 * uint32_t seems like overkill for the vout data-type, but this is the type used by Core -
 * see: https://developer.bitcoin.org/reference/transactions.html
 * */
void DBWrapper::fetchRecord(const std::string& txid, const uint32_t vout, BytesVec& value)
{
	std::vector<char> keyBytes;
	utilities::hexstringToBytes(txid, keyBytes);
	utilities::switchEndianness(keyBytes);
	keyBytes.insert(keyBytes.begin(), 0x43);
	keyBytes.insert(keyBytes.end(), (unsigned char)vout); // vout
	leveldb::Slice keySlice(keyBytes.data(), keyBytes.size());
	std::string rawVal;
	
	status = db->Get(readoptions, keySlice, &rawVal);
		if (!status.ok()) {
		throw std::invalid_argument("Key not found.");
	}
	
	deObfuscate(rawVal, value);
}

void DBWrapper::fetchRecord(const std::string& txid, const uint32_t vout, std::string& value)
{
	BytesVec deObBytes;
	fetchRecord(txid, vout, deObBytes);
	utilities::bytesToHexstring(deObBytes, value);
}
