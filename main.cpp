#include <iostream>
#include <cassert>
#include <string>
#include "leveldb/db.h"

int main()
{

	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, "testdb", &db);
	if (!status.ok()) std::cerr << status.ToString() << std::endl;
	assert(status.ok());

	// Write to db
	std::string key = "name";
	std::string value = "David";
	status = db->Put(leveldb::WriteOptions(), key, value);
	assert(status.ok());
	
	// Read from db
	std::string val_back;
	status = db->Get(leveldb::ReadOptions(), key, &val_back);
	assert(status.ok());

	std::cout << key << ": " << val_back << "\n";
	return 0;
}
