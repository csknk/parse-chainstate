#ifndef DBWRAPPER_H
#define DBWRAPPER_H 

#include <iostream>
#include <vector>
#include <stdexcept>
#include "leveldb/db.h"

class DBWrapper {
private:
	std::string obfuscationKeyKey;
	std::vector<unsigned char> obfuscationKey;
	leveldb::Options options;
	std::string dbName;
	leveldb::ReadOptions readoptions;
	leveldb::DB* db;
	leveldb::Status status;
	void checkStatus(std::string msg);
	void setObfuscationKey();
	void openDB();
	void deObfuscate(leveldb::Slice bytes, std::vector<unsigned char>& plaintext); 

public:
	DBWrapper();
	DBWrapper(std::string dbName);
	void setDBName(const std::string& s);
	void read(std::string const& key, std::string& val);
	void outputAllKeyVals();
	void fetchRecord(const std::string& key, std::string& value);
};

#endif /* DBWRAPPER_H */
