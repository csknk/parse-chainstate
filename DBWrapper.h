#ifndef DBWRAPPER_H
#define DBWRAPPER_H 

#include <iostream>
#include <vector>
#include <stdexcept>
#include "utilities.h"
#include "leveldb/db.h"


class DBWrapper {
private:
	std::string obfuscationKeyKey;
	std::vector<unsigned char> obfuscationKey;
	leveldb::Options options;
	std::string dbName;
	leveldb::ReadOptions readoptions;
	leveldb::DB* db;
	void checkStatus(leveldb::Status status, std::string msg);
	void setObfuscationKey();
	void openDB();

public:
	DBWrapper();
	DBWrapper(std::string dbName);
	void setDBName(const std::string& s);
	void read(std::string const& key, std::string& val);
};


#endif /* DBWRAPPER_H */
