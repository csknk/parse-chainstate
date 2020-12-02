#include "dbwrapper.h"
#include "utilities.h"

DBWrapper::DBWrapper(std::string _dbName) : dbName(_dbName)
{
	try {
		openDB();
	} catch (const std::invalid_argument& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	} catch (const char* msg) {
		std::cerr << msg << '\n';
		exit(EXIT_FAILURE);
	}
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
	utilities::printToHex(obfuscationKey);
}

void DBWrapper::openDB()
{
	if (dbName.empty()) {
		throw std::invalid_argument{"No database specified in DBWrapper::openDB()"};
	}
	options.create_if_missing = false;
	leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
	if (!status.ok()) {
		throw "Can't open the specified database.";
	}
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
 *
 * */
void DBWrapper::getAllUTXOs(std::vector<UTXO>& utxos)
{
	leveldb::Iterator* it = db->NewIterator(readoptions);
	for (it->SeekToFirst(); it->Valid(); it->Next()) {

		BytesVec key;
		for (size_t i = 0; i < it->key().size(); i++) {
			key.push_back(it->key()[i]);
		} 
		if (key[0] == 0x43) {
			BytesVec deObfuscatedValue;
			deObfuscate(it->value(), deObfuscatedValue);
			Varint v(deObfuscatedValue);
			UTXO u(v);
			BytesVec txid;
			assert(key.size() > 33);
			txid.insert(txid.begin(), key.begin() + 1, key.begin() + 33);
			utilities::switchEndianness(txid);
			u.setTXID(txid);
			utxos.push_back(u);
		}
	}
	assert(it->status().ok());
	delete it;
}

void DBWrapper::printAllUTXOs()
{
	leveldb::Iterator* it = db->NewIterator(readoptions);
	for (it->SeekToFirst(); it->Valid(); it->Next()) {

		BytesVec key;
		for (size_t i = 0; i < it->key().size(); i++) {
			key.push_back(it->key()[i]);
		} 
		if (key[0] == 0x43) {
			BytesVec deObfuscatedValue;
			deObfuscate(it->value(), deObfuscatedValue);
			Varint v(deObfuscatedValue);
			UTXO u(v);
			BytesVec txid;
			assert(key.size() > 33);
			txid.insert(txid.begin(), key.begin() + 1, key.begin() + 33);
			utilities::switchEndianness(txid);
			u.setTXID(txid);
			std::string output;
			u.csv(output);
			std::cout << output;
//			std::cout << u << "\n";
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
