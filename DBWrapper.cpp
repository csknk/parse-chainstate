#include "DBWrapper.h"

DBWrapper::DBWrapper(std::string _dbName) : dbName(_dbName)
{
	setObfuscationKey();
	openDB();
}

void DBWrapper::setObfuscationKey()
{
	obfuscationKeyKey = {0x0e , 0x00};
	obfuscationKeyKey += "obfuscate_key";
	std::string obfuscationKeyString;
	read(obfuscationKeyKey, obfuscationKeyString);
	utilities::stringToHexBytes(obfuscationKeyString, obfuscationKey);

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
	leveldb::Status status = db->Get(readoptions, key, &val);
	checkStatus(status, "Error reading obfuscation key");
}

void DBWrapper::checkStatus(leveldb::Status status, std::string msg)
{
	if (!status.ok()) {
		std::cerr << msg << "\n";
		std::cerr << status.ToString() << std::endl;
	}
	assert(status.ok());
	
}

